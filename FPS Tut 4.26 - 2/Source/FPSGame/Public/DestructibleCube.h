// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleCube.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCubeHit); //Declaring cube multicast delegates

UCLASS()
class FPSGAME_API ADestructibleCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleCube();

	UFUNCTION(BlueprintCallable)
	void GetHit(); //spawn 4 smaller cubes

	UFUNCTION(BlueprintCallable)
	void GetHitCharge(float scale); //destroy nearby cubes(or just change color)

	//sets cube as small cube, will probably use for starting delegate idk
	void SetSmallCube();

	UFUNCTION()
	void ResizeCube();
	
	UFUNCTION()
	void RecolorCube();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mesh;

	UPROPERTY(VisibleAnywhere, Category = "Destruction")
	float explosionRadius;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FCubeHit CubeDelegate;

	bool isSmallerCube = false;

	void SpawnSmallCube(FVector loc, FVector scale);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
