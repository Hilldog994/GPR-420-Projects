// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBombActor.generated.h"

UCLASS()
class FPSGAME_API AFPSBombActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSBombActor();

	//Explodes bomb, public so that it can be exploded instantly
	UFUNCTION(BlueprintCallable)
	void ExplodeBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	float ExplodeDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	UParticleSystem* ExplosionTemplate;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	float bombRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	float bombStrength;

	UMaterialInstanceDynamic* matInst;

	FLinearColor initialColor;
	FLinearColor targetColor;



	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* mesh;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
