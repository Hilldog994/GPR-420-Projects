// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
//#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Kismet/GameplayStatics.h"

bool UFunctionLibrary::DetermineIfEnemiesKilled(AActor* actor, TSubclassOf<AActor> actorClass)
{
	UWorld* world = actor->GetWorld();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(world, actorClass, FoundActors);

	int numEnemies = FoundActors.Num();

	if (numEnemies <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}