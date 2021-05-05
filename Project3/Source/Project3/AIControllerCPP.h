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
protected:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* perceptionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	TSubclassOf<ACharacter> playerClass;

	virtual void BeginPlay() override;

	UFUNCTION()
	void SensePlayer(AActor* Actor, FAIStimulus Stimulus);
public:
	AAIControllerCPP();
};
