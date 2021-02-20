// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleCube.h"

// Sets default values
ADestructibleCube::ADestructibleCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = mesh;

	//setup collision to be same as editor cubes
	mesh->SetSimulatePhysics(true);

	mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	explosionRadius = 500.f;

}

//spawns 4 smaller cubes,destroy original
void ADestructibleCube::GetHit()
{
	FVector scale = RootComponent->GetComponentScale();

	if (scale.GetMin() <= 0.75f)
	{
		Destroy();
	}
	else
	{
		ADestructibleCube* spawnedCube1 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(1.0, 0.0, 1.0), GetActorRotation());
		ADestructibleCube* spawnedCube2 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(1.0, 0.0, -1.0), GetActorRotation());
		ADestructibleCube* spawnedCube3 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(-1.0, 0.0, 1.0), GetActorRotation());
		ADestructibleCube* spawnedCube4 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(-1.0, 0.0, -1.0), GetActorRotation());
		scale *= .5f; //half of all dimensions makes a 4th of the cube
		spawnedCube1->RootComponent->SetWorldScale3D(scale);
		spawnedCube2->RootComponent->SetWorldScale3D(scale);
		spawnedCube3->RootComponent->SetWorldScale3D(scale);
		spawnedCube4->RootComponent->SetWorldScale3D(scale);

		Destroy();
	}
	
}

void ADestructibleCube::GetHitCharge()
{
	TArray<FOverlapResult> overlaps;

	FCollisionObjectQueryParams queryParams;
	queryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	queryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionShape shape;
	shape.SetSphere(explosionRadius);

	GetWorld()->OverlapMultiByObjectType(overlaps, GetActorLocation(), FQuat::Identity, queryParams, shape);

	for (FOverlapResult result : overlaps)
	{
		UPrimitiveComponent* overlapComp = result.GetComponent();
		if (overlapComp && overlapComp->IsSimulatingPhysics())
		{
			if (Cast<ADestructibleCube>(overlapComp->GetOwner()))
			{
				//Destroy(overlapComp->GetOwner());

				UMaterialInstanceDynamic* mat = overlapComp->CreateAndSetMaterialInstanceDynamic(0);
				if (mat)
				{
					mat->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
				}
			}
		}
	}
	Destroy();
}

// Called when the game starts or when spawned
void ADestructibleCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADestructibleCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

