// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "HttpActor.generated.h"

UCLASS()
class FPSGAME_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpActor();

	FHttpModule* http;

	FString windDir;

	int32 windSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<int32> windSpeeds;
	TArray<FString> windDirections;

	//how many real time minutes equate to an hour(aka how long till update to next hour)
	int minutesPerHour = 1;

	//how many hours to pull from api, will be 60/minutes per hour. 
	//Since a real hour will have passed by time everything has gone through, api will have updated
	int amtHoursToPull;

	//what hour to be looking at in the forcast
	int currentInGameHour = 0;

	UFUNCTION()
	void UpdateValues();

public:	
	UFUNCTION()
	void MyHttpCall();

	void HttpRequest(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccess);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
