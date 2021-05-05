// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerCPP.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"


AAIControllerCPP::AAIControllerCPP()
{
	perceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	PrimaryActorTick.bCanEverTick = true;
	moveToAcceptanceRadius = 50.f;
}

void AAIControllerCPP::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (Result.Code == EPathFollowingResult::Success)
	{
		if (currentMoveType == MoveType::PLAYERFOLLOW)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Reached Target"));
			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionTemplate, GetPawn()->GetActorLocation(),FRotator::ZeroRotator,GetPawn()->GetActorScale());//spawn particle system
			TArray<AActor*> ignored;
			UGameplayStatics::ApplyRadialDamage(this, explodeDamage, GetPawn()->GetActorLocation(), explodeRadius, UDamageType::StaticClass(), ignored, this, GetInstigatorController(), true);
			if (GetPawn())
			{
				GetPawn()->Destroy();
			}
		}
		else if (currentMoveType == MoveType::SELFMOVE)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Back to Original"));
			GetPawn()->SetActorRotation(startingRotation);//goes back to starting rotation when reaching original position
			//SetControlRotation(startingRotation);
		}
	}
	/*else if(Result.Code == EPathFollowingResult::Blocked)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, Result.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Interupted"));
		//StopMovement();
		MoveToLocation(startingPosition, moveToAcceptanceRadius);//move back to starting location
		currentMoveType = MoveType::SELFMOVE;//set correct type for checking when move is complete correctly
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SELFMOVE"));
	}*/
}

void AAIControllerCPP::BeginPlay()
{
	Super::BeginPlay();
	//cast event for when ai percieves something
	perceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerCPP::SensePlayer);
	startingPosition = GetPawn()->GetActorLocation();//get start location
	startingRotation = GetPawn()->GetActorRotation();//get start location
}

void AAIControllerCPP::SensePlayer(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())//on sight
	{
		if (Actor->GetClass() == playerClass)//check its the player
		{
			playerObject = Actor;
			MoveToActor(playerObject, moveToAcceptanceRadius);//start moving toward player
			currentMoveType = MoveType::PLAYERFOLLOW; //set correct move type for when its complete
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("PLAYERFOLLOW"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Detected"));
		}
	}
	else //on lose sight
	{
		if (Actor->GetClass() == playerClass)//check its the player
		{
			playerObject = nullptr;
			//StopMovement();//stop moving if player out of range
			MoveToLocation(startingPosition, -1);//move back to starting location
			//if(res == EPathFollowingRequestResult::RequestSuccessful)
			currentMoveType = MoveType::SELFMOVE; //set correct move type for when its complete
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Left Sight"));
		}
	}
}