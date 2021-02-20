// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleCube.generated.h"

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
	void GetHitCharge(); //destroy nearby cubes(or just change color)


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mesh;

	UPROPERTY(VisibleAnywhere, Category = "Destruction")
	float explosionRadius;

	bool isSmallerCube = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
