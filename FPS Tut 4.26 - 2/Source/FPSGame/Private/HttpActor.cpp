#include "HttpActor.h"

// Sets default values
AHttpActor::AHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	http = &FHttpModule::Get();

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();
	MyHttpCall();	
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AHttpActor::MyHttpCall, 60.f, true); //check JSON every minute
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
		//https://answers.unrealengine.com/questions/390663/how-to-retrieve-single-text-field-from-json.html showing that you keep getting fields
		//https://forums.unrealengine.com/development-discussion/c-gameplay-programming/59874-json-help-with-complex-structures getting array field as object
		//periods[0] is this afternoon, 1 is tonight, 2 is next day, 3 is next night, etc.
		temperature = jsonObject->GetObjectField("properties")->GetArrayField("periods")[0]->AsObject()->GetIntegerField("temperature");
		windDir = jsonObject->GetObjectField("properties")->GetArrayField("periods")[0]->AsObject()->GetStringField("windDirection");
		FString windSpdFull = jsonObject->GetObjectField("properties")->GetArrayField("periods")[0]->AsObject()->GetStringField("windSpeed");
		//https://docs.unrealengine.com/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/StringHandling/FString/index.html#encodingconversionmacros
		//fstring to int to convert wind speed to an int to use
		windSpeed =  FCString::Atoi(*windSpdFull.Left(2));

	}
}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

