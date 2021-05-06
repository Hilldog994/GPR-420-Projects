// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionLibrary.generated.h"

UCLASS()
class PROJECT3_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		UFUNCTION(BlueprintCallable, Category = "MyCategory")
		static bool DetermineIfEnemiesKilled(AActor* actor, TSubclassOf<AActor> actorClass);
};
