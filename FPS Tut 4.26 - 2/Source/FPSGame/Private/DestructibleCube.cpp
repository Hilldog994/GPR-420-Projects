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
		//tried disabling collision before spawning to prevent them flying out but it didnt change anything
		SetActorEnableCollision(false);
		//spawns 4 cubes
		ADestructibleCube* spawnedCube1 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(80.0, 0.0, 150.0), GetActorRotation());
		spawnedCube1->RootComponent->SetWorldScale3D(scale);
		spawnedCube1->isSmallerCube = true;
		//spawnedCube1->SetActorEnableCollision(false);

		ADestructibleCube* spawnedCube2 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(80.0, 0.0, 0.0), GetActorRotation());
		//spawnedCube2->SetActorEnableCollision(false);
		spawnedCube2->RootComponent->SetWorldScale3D(scale);
		spawnedCube2->isSmallerCube = true;

		ADestructibleCube* spawnedCube3 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(-80.0, 0.0, 150.0), GetActorRotation());
		//spawnedCube3->SetActorEnableCollision(false);
		spawnedCube3->RootComponent->SetWorldScale3D(scale);
		spawnedCube3->isSmallerCube = true;

		ADestructibleCube* spawnedCube4 = GetWorld()->SpawnActor<ADestructibleCube>(this->GetClass(), GetActorLocation() + FVector(-80.0, 0.0, 0.0), GetActorRotation());
		//spawnedCube4->SetActorEnableCollision(false);
		spawnedCube4->RootComponent->SetWorldScale3D(scale);
		spawnedCube4->isSmallerCube = true;
		
		
		
		
		

		Destroy();//destroys original cube(this)
	}
	
}

//When charge shot hits cube, change color or destroy nearby cubes
void ADestructibleCube::GetHitCharge()
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
				//overlapComp->GetOwner()->Destroy(); //destroy overlap cube
				
				//change color to random color
				overlapComp->AddRadialImpulse(GetActorLocation(), explosionRadius, 5000.f, ERadialImpulseFalloff::RIF_Linear,true);
				UMaterialInstanceDynamic* mat = overlapComp->CreateAndSetMaterialInstanceDynamic(0);
				if (mat)
				{
					mat->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
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

// Called every frame
void ADestructibleCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

