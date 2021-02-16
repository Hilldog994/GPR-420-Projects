// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "FPSBombActor.h"

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

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}


void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		FVector scale = OtherComp->GetComponentScale();
		scale *= .8f;
		if (scale.GetMin() < 0.5f)
		{
			//destroys cube
			OtherActor->Destroy();

			//spawns a bomb, then explodes it
			AFPSBombActor* mBomb = GetWorld()->SpawnActor<AFPSBombActor>(BombClass, GetActorLocation(), GetActorRotation());
			mBomb->ExplodeBomb();


		}
		else
		{
			//spawn 4 smaller cubes
			/*AActor* spawnedCube1 = GetWorld()->SpawnActor<AActor>(CubeClass, OtherActor->GetActorLocation(), OtherActor->GetActorRotation());
			AActor* spawnedCube2 = GetWorld()->SpawnActor<AActor>(CubeClass, OtherActor->GetActorLocation(), OtherActor->GetActorRotation());
			AActor* spawnedCube3 = GetWorld()->SpawnActor<AActor>(CubeClass, OtherActor->GetActorLocation(), OtherActor->GetActorRotation());
			AActor* spawnedCube4 = GetWorld()->SpawnActor<AActor>(CubeClass, OtherActor->GetActorLocation(), OtherActor->GetActorRotation());
			scale *= .25f;
			spawnedCube1->SetActorScale3D(scale);
			spawnedCube2->SetActorScale3D(scale);
			spawnedCube3->SetActorScale3D(scale);
			spawnedCube4->SetActorScale3D(scale);*/
			OtherComp->SetWorldScale3D(scale);

			//destroys cube
			//OtherActor->Destroy();
		}

		UMaterialInstanceDynamic* MatInstance = OtherComp->CreateAndSetMaterialInstanceDynamic(0);

		if (MatInstance)
		{
			MatInstance->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
		}

		Destroy();
	}
}