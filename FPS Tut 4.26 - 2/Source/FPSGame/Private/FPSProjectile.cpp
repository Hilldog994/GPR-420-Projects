// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "FPSBombActor.h"
#include "DestructibleCube.h"

AFPSProjectile::AFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	PrimaryActorTick.bCanEverTick = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	windForce = FVector::ZeroVector;
}


void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		//if hit destructible cube
		if (Cast<ADestructibleCube>(OtherActor))
		{
			//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			Cast<ADestructibleCube>(OtherActor)->GetHit();//call get hit for cube
		}
		
		UMaterialInstanceDynamic* MatInstance = OtherComp->CreateAndSetMaterialInstanceDynamic(0);

		if (MatInstance)
		{
			MatInstance->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
		}

		Destroy();


	}
}

void AFPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ProjectileMovement->AddForce(windForce);
}

void AFPSProjectile::ApplyWind(FString direction, int speed)
{
	if (direction == "N")
	{
		windForce = FVector(1, 0, 0);
	}
	else if (direction == "S")
	{
		windForce = FVector(-1, 0, 0);
	}
	else if (direction == "E")
	{
		windForce = FVector(0, 1, 0);
	}
	else if (direction == "W")
	{
		windForce = FVector(0, -1, 0);
	}
	else if (direction == "NE")
	{
		windForce = FVector(1, 1, 0);
	}
	else if (direction == "SE")
	{
		windForce = FVector(-1, 1, 0);
	}
	else if (direction == "SW")
	{
		windForce = FVector(-1, -1, 0);
	}
	else if (direction == "NW")
	{
		windForce = FVector(1, -1, 0);
	}
	windForce.Normalize();
	windForce *= speed * 120;
}