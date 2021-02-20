// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSChargeShotProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "FPSBombActor.h"
#include "DestructibleCube.h"
#include "Kismet/GameplayStatics.h"

AFPSChargeShotProjectile::AFPSChargeShotProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeComp"));
	//CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSChargeShotProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 6000.0f;
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; //make it a straight shot

	// Die after 1 seconds by default
	InitialLifeSpan = 1.0f;
}


void AFPSChargeShotProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		//if hit destructible cube
		if (Cast<ADestructibleCube>(OtherActor))
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, ChargeShotPart, GetActorLocation());//spawn particle system
			OtherComp->AddImpulseAtLocation(GetVelocity() * 150.0f, GetActorLocation());
			Cast<ADestructibleCube>(OtherActor)->GetHitCharge();//call get hit for the charged shot for cube
		}
		
		UMaterialInstanceDynamic* MatInstance = OtherComp->CreateAndSetMaterialInstanceDynamic(0);

		if (MatInstance)
		{
			MatInstance->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
		}

		Destroy();


	}
	else
	{
		Destroy();
	}

}