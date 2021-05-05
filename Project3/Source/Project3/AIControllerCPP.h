// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GameFramework/Character.h"
#include "AIControllerCPP.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT3_API AAIControllerCPP : public AAIController
{
	GENERATED_BODY()

	enum MoveType
	{
		PLAYERFOLLOW,
		SELFMOVE
	};
protected:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* perceptionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	TSubclassOf<ACharacter> playerClass;

	UPROPERTY(BlueprintReadOnly,Category = "Setup")
	AActor* playerObject;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UParticleSystem* ExplosionTemplate;

	FVector startingPosition;
	FRotator startingRotation;
	MoveType currentMoveType;

	virtual void BeginPlay() override;

	UFUNCTION()
	void SensePlayer(AActor* Actor, FAIStimulus Stimulus);
public:
	AAIControllerCPP();

	//When movement is completed this is automatically called
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float moveToAcceptanceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float explodeDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float explodeRadius;

};
