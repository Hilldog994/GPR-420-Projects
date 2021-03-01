// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimSequence.h"
#include "FPSBombActor.h"
#include "FPSChargeShotProjectile.h"


AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(0, 0, BaseEyeHeight)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->SetRelativeRotation(FRotator(2.0f, -15.0f, 5.0f));
	Mesh1PComponent->SetRelativeLocation(FVector(0, 0, -160.0f));

	// Create a gun mesh component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");

	http = &FHttpModule::Get();
}

void AFPSCharacter::MyHttpCall()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = http->CreateRequest();
	request->OnProcessRequestComplete().BindUObject(this, &AFPSCharacter::HttpRequest);
	request->SetURL("https://api.weather.gov/gridpoints/LWX/96,70/forecast");
	request->SetVerb("GET");
	request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	request->SetHeader("Content-Type", TEXT("application/json"));
	request->ProcessRequest();
}

void AFPSCharacter::HttpRequest(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccess)
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
		int32 recievedInt = jsonObject->GetObjectField("properties")->GetArrayField("periods")[0]->AsObject()->GetIntegerField("temperature");

		//Output
		GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Green, FString::FromInt(recievedInt));
	}
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	
	PlayerInputComponent->BindAction("RestartLevel", IE_Pressed, this, &AFPSCharacter::RestartLevel);
	
	PlayerInputComponent->BindAction("Charge", IE_Pressed, this, &AFPSCharacter::StartCharge);
	PlayerInputComponent->BindAction("Charge", IE_Released, this, &AFPSCharacter::FireChargeShot);

	PlayerInputComponent->BindAction("Bomb", IE_Pressed, this, &AFPSCharacter::SpawnBomb);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AFPSCharacter::RestartLevel()
{
	FTimerDelegate timerDel;
	FTimerHandle timerHand;

	timerDel.BindUFunction(this, FName("DelegateRestart"));
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Starting Reset"));
	//reset in 3 seconds
	GetWorldTimerManager().SetTimer(timerHand, timerDel, 3.f, false);
}

void AFPSCharacter::DelegateRestart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Start Charge"));
	GetWorld()->GetFirstPlayerController()->RestartLevel();
}

void AFPSCharacter::Fire()
{
	// try and fire a projectile
	if (ProjectileClass)
	{
		// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		// Use controller rotation which is our view direction in first person
		FRotator MuzzleRotation = GetControlRotation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	}

	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
		}
	}
}

void AFPSCharacter::StartCharge()
{
	if (allowCharge) //if charge shot off cooldown
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Start Charge"));
		GetWorldTimerManager().SetTimer(chargeHoldHandle, this, &AFPSCharacter::SetCharge, 1.f, false); //make it so able to charge shot after 1 second of holding
	}
}

void AFPSCharacter::FireChargeShot()
{
	// try and fire a projectile
	if (ChargeShotClass)
	{
		if (chargingDone) //if shot is fully charged, perform the shot
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Charge Shot"));
			chargingDone = false; //reset charge

			// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
			FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
			// Use controller rotation which is our view direction in first person
			FRotator MuzzleRotation = GetControlRotation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			GetWorld()->SpawnActor<AFPSChargeShotProjectile>(ChargeShotClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);

			// try and play the sound if specified
			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (FireAnimation)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
				}
			}
			//start 3 second cooldown of the charge shot
			allowCharge = false;
			FTimerHandle chargeCooldownHandle;
			GetWorldTimerManager().SetTimer(chargeCooldownHandle, this, &AFPSCharacter::AllowCharge, 3.f, false); //make it so able to charge shot again after 3 second
		}
		else
		{
			//reset the charge shot timer since released too early
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Charge Fail"));
			GetWorldTimerManager().ClearTimer(chargeHoldHandle);
		}
	}
}

void AFPSCharacter::SpawnBomb()
{
	AFPSBombActor* mBomb = GetWorld()->SpawnActor<AFPSBombActor>(BombClass,GetActorLocation(),GetActorRotation());
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	MyHttpCall();
}

/*void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}*/

