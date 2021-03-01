// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class AFPSBombActor;
class AFPSChargeShotProjectile;

UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()
protected:

	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMeshComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	bool chargingDone = false;
	bool allowCharge = true;

public:
	AFPSCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AFPSChargeShotProjectile> ChargeShotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Bombs")
	TSubclassOf<AFPSBombActor> BombClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;

	FTimerHandle chargeHoldHandle;
	
	FHttpModule* http;
protected:
	
	/** Fires a projectile. */
	void Fire();

	//restarts level
	void RestartLevel();

	UFUNCTION()
	void DelegateRestart();

	//fires the charged shot projectile
	void FireChargeShot();

	//Starts the charging of charge shot
	void StartCharge();

	//Spawns a bomb
	void SpawnBomb();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	//Indicates that the charge shot is done charging up
	void SetCharge() { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Charge Done")); chargingDone = true; }
	//Makes it so charge shot can be performed again
	void AllowCharge(){ GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Charge Ready")); allowCharge = true; }

public:
	UFUNCTION()
	void MyHttpCall();

	void HttpRequest(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccess);

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent; }

};

