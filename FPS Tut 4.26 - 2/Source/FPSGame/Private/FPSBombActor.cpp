// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBombActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "DestructibleCube.h"

// Sets default values
AFPSBombActor::AFPSBombActor()
{
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	RootComponent = mesh;

	mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ExplodeDelay = 2.0f;
	bombRadius = 500.0f;
	bombStrength = 200.0f;
}

// Called when the game starts or when spawned
void AFPSBombActor::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle bombTimerHandle;
	GetWorldTimerManager().SetTimer(bombTimerHandle, this, &AFPSBombActor::ExplodeBomb, ExplodeDelay); //call explode bomb after delay

	matInst = mesh->CreateAndSetMaterialInstanceDynamic(0);

	if (matInst)
	{
		initialColor = matInst->K2_GetVectorParameterValue("Color");
	}

	targetColor = FLinearColor::MakeRandomColor();
}

void AFPSBombActor::ExplodeBomb()
{
	TArray<FOverlapResult> overlaps;

	FCollisionObjectQueryParams queryParams;
	queryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	queryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionShape shape;
	shape.SetSphere(bombRadius);

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionTemplate, GetActorLocation());//spawn particle system
	GetWorld()->OverlapMultiByObjectType(overlaps, GetActorLocation(), FQuat::Identity, queryParams, shape);

	for (FOverlapResult result : overlaps)
	{
		UPrimitiveComponent* overlapComp = result.GetComponent();
		if (overlapComp && overlapComp->IsSimulatingPhysics())
		{
			if (Cast<ADestructibleCube>(overlapComp->GetOwner()))
			{
				//overlapComp->AddRadialImpulse(GetActorLocation(), bombRadius, bombStrength, ERadialImpulseFalloff::RIF_Linear, true);
				Cast<ADestructibleCube>(overlapComp->GetOwner())->GetHit();//call get hit for cube
			}

			UMaterialInstanceDynamic* mat = overlapComp->CreateAndSetMaterialInstanceDynamic(0);
			if (mat)
			{
				mat->SetVectorParameterValue("Color", targetColor);
			}

		}
	}


	Destroy();
}

// Called every frame
void AFPSBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//interpolate color as explosion happens
	if (matInst)
	{
		float progress = (GetWorld()->TimeSeconds - CreationTime) / ExplodeDelay;
		FLinearColor newColor = FLinearColor::LerpUsingHSV(initialColor, targetColor,progress);
		matInst->SetVectorParameterValue("Color", newColor);
	}

}

