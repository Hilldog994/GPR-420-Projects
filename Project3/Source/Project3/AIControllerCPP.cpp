// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerCPP.h"
#include "Perception/AIPerceptionComponent.h"


AAIControllerCPP::AAIControllerCPP()
{
	perceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	PrimaryActorTick.bCanEverTick = true;
}

void AAIControllerCPP::BeginPlay()
{
	Super::BeginPlay();
	perceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerCPP::SensePlayer);
}

void AAIControllerCPP::SensePlayer(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())//on sight
	{
		if (Actor->GetClass() == playerClass)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Detected"));
		}
	}
	else
	{
		if (Actor->GetClass() == playerClass)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Left Sight"));
		}
	}
}