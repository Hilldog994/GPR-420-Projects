#include "HttpActor.h"

// Sets default values
AHttpActor::AHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	http = &FHttpModule::Get();

	amtHoursToPull = 60 / minutesPerHour;
}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();
	MyHttpCall();	
	FTimerHandle handle;
	//update the weather variables to the next hour when an in game hour passes
	GetWorldTimerManager().SetTimer(handle, this, &AHttpActor::UpdateValues, 60 * minutesPerHour, true);
}

void AHttpActor::UpdateValues()
{
	currentInGameHour++;//will be called when hour has passed so increase current hour
	if (currentInGameHour >= amtHoursToPull) //check if real hour has passed yet
	{
		MyHttpCall();//re-assign values from HTTP since they will be different now
		currentInGameHour = 0; //go back to beginning
	}
	//set values for wind for current hour in game
	windSpeed = windSpeeds[currentInGameHour];
	windDir = windDirections[currentInGameHour];
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "New Wind Direction: " + windDir);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "New Wind Speed: " + FString::FromInt(windSpeed));
}

void AHttpActor::MyHttpCall()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = http->CreateRequest();
	request->OnProcessRequestComplete().BindUObject(this, &AHttpActor::HttpRequest);
	//hourly forcast JSON URL
	request->SetURL("https://api.weather.gov/gridpoints/LWX/96,70/forecast/hourly");
	request->SetVerb("GET");
	request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	request->SetHeader("Content-Type", TEXT("application/json"));
	request->ProcessRequest();
}

void AHttpActor::HttpRequest(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccess)
{
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> jsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(reader, jsonObject))
	{
		//reset array's values
		windDirections.Empty();
		windSpeeds.Empty();

		//https://answers.unrealengine.com/questions/390663/how-to-retrieve-single-text-field-from-json.html showing that you keep getting fields
		//https://forums.unrealengine.com/development-discussion/c-gameplay-programming/59874-json-help-with-complex-structures getting array field as object
		for (int i = 1; i <= amtHoursToPull; i++)
		{
			//gets wind values from JSON, each new period is forcast an hour later
			FString tempWindDir = jsonObject->GetObjectField("properties")->GetArrayField("periods")[i]->AsObject()->GetStringField("windDirection");
			FString windSpdFull = jsonObject->GetObjectField("properties")->GetArrayField("periods")[i]->AsObject()->GetStringField("windSpeed");
			//https://docs.unrealengine.com/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/StringHandling/FString/index.html#encodingconversionmacros
			//fstring to int to convert wind speed to an int to use, accurate as long as not triple digits which is practically impossible
			int tempWindSpeed = FCString::Atoi(*windSpdFull.Left(2));

			//populate arrays with wind data
			windDirections.Add(tempWindDir);
			windSpeeds.Add(tempWindSpeed);
		}
		//initialize wind variables
		windSpeed = windSpeeds[0];
		windDir = windDirections[0];
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "Wind Direction: " + windDir);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "Wind Speed: " + FString::FromInt(windSpeed));
	}

}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

