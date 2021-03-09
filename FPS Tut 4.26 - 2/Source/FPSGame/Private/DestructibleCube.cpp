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
	//destroy if one of the smaller 4 cubes
	if (isSmallerCube)//scale.GetMin() <= 0.75f used to be here but big cubes could split again which isnt what instructions say should happen
	{
		Destroy();
	}
	else
	{
		scale *= .5f; //half of all dimensions makes a 4th of the cube, apply to all spawned cubes
		//disable collision so smaller cubes dont propel themselves when spawned
		SetActorEnableCollision(false);
		//spawns 4 smaller cubes
		SpawnSmallCube(GetActorLocation() + FVector(80.0, 0.0, 150.0), scale);

		SpawnSmallCube(GetActorLocation() + FVector(80.0, 0.0, 0.0), scale);

		SpawnSmallCube(GetActorLocation() + FVector(-80.0, 0.0, 150.0), scale);

		SpawnSmallCube(GetActorLocation() + FVector(-80.0, 0.0, 0.0), scale);
		

		Destroy();//destroys original cube(this)
	}
	
}

void ADestructibleCube::SpawnSmallCube(FVector loc, FVector scale)
{
	ADestructibleCube* spawnedCube = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), loc, GetActorRotation());
	spawnedCube->RootComponent->SetWorldScale3D(scale);
	spawnedCube->SetSmallCube();
}

//When charge shot hits cube, change color or destroy nearby cubes
void ADestructibleCube::GetHitCharge(float scale)
{
	TArray<FOverlapResult> overlaps;

	FCollisionObjectQueryParams queryParams;
	queryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	queryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionShape shape;
	shape.SetSphere(explosionRadius);

	GetWorld()->OverlapMultiByObjectType(overlaps, GetActorLocation(), FQuat::Identity, queryParams, shape);
	//overlap within radius
	for (FOverlapResult result : overlaps)
	{
		UPrimitiveComponent* overlapComp = result.GetComponent();
		if (overlapComp && overlapComp->IsSimulatingPhysics())
		{
			if (Cast<ADestructibleCube>(overlapComp->GetOwner())) //if overlap is a destructible cube
			{
				ADestructibleCube* cube = Cast<ADestructibleCube>(overlapComp->GetOwner());
				//overlapComp->GetOwner()->Destroy(); //destroy overlap cube
				
				//change color to random color
				overlapComp->AddRadialImpulse(GetActorLocation(), explosionRadius, 5000.f, ERadialImpulseFalloff::RIF_Linear,true);
				float newScale = FMath::FRandRange(overlapComp->GetComponentScale().X * 1.0f/ scale, overlapComp->GetComponentScale().X);
				overlapComp->SetWorldScale3D(FVector(newScale));
				UMaterialInstanceDynamic* mat = overlapComp->CreateAndSetMaterialInstanceDynamic(0);
				if (mat)
				{
					mat->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
				}
				if (cube->isSmallerCube) //if already small, destroy
				{
					overlapComp->GetOwner()->Destroy();
				}
				else
				{
					cube->SetSmallCube();
				}
				

			}
		}
	}
	Destroy();//destroy original attacked cube(this)
}

// Called when the game starts or when spawned
void ADestructibleCube::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADestructibleCube::SetSmallCube()
{
	isSmallerCube = true;
}

// Called every frame
void ADestructibleCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

