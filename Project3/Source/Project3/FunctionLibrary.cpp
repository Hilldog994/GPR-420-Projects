// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//Returns true if all enemies in the room are killed. Returns false if there are still some left
bool UFunctionLibrary::DetermineIfEnemiesKilled(AActor* actor, TSubclassOf<AActor> actorClass)
{
	//Determines world based on given actor
	UWorld* world = actor->GetWorld(); 

	//Stores all enemies in an array
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(world, actorClass, FoundActors);

	//Determines length of array. If 0 (no enemies found) returns true
	int numEnemies = FoundActors.Num();
	if (numEnemies <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}