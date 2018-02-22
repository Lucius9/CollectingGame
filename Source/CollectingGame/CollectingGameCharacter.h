// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Net/UnrealNetwork.h" 
#include "CollectingGameCharacter.generated.h"

UENUM(BlueprintType)
enum class ECameraTypeEnum : uint8
{
	VE_FPS	UMETA(DisplayName="FPS"),
	VE_TPS	UMETA(DisplayName="TPS")
};

UCLASS(config=Game)
class ACollectingGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;	

	/** FPS camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCamera;	

	/*Current Active Camera*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* CurrentCamera;

	/** Physic Handle to pick up objects */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Object Picking", meta= (AllowPrivateAccess = "true"))
	class UPhysicsHandleComponent* HandleComponent;

	/** SceneComponent to set up location of the held objects */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly,Category = "Object Picking", meta = (AllowPrivateAccess = "true"))
	class USceneComponent *HeldObjectLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponMesh;	

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "CameraType", meta = (AllowPrivateAccess = "true"))
	ECameraTypeEnum CameraTypeEnum;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bJumpPressed = false;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCrouchPressed = false;
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCanMove = true;	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCameraMovement = true;
	UPROPERTY(Replicated)
	bool bholdingObject = false;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsInAimOffsetRotation = false;


	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ACollectingProjectile> ProjectileClass;

public:
	ACollectingGameCharacter();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const ;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	virtual void Tick(float deltaSeconds);

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	virtual void AddControllerYawInput(float val)override;

	virtual void AddControllerPitchInput(float val)override;
	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);	

	virtual void Jump() override;	
	virtual void StopJumping() override;

	void OnCrouchPressed();	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	void OnPickUpKeyPressed();

	//Pick Up Object 		
	void PickUpObject();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPickUpObject();
	virtual void ServerPickUpObject_Implementation();
	virtual bool ServerPickUpObject_Validate();

	//Drop Object	
	void DropObject();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropObject();
	virtual void ServerDropObject_Implementation();
	virtual bool ServerDropObject_Validate();
	void SwitchCamera();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns isHoldingObject*/
	FORCEINLINE bool IsHoldingObject() { return bholdingObject; }

	FORCEINLINE ECameraTypeEnum GetCameraType() { return CameraTypeEnum ; }

	void SetCameraType(ECameraTypeEnum SomeCameraTypeEnum);	


	UFUNCTION(Server, Reliable, WithValidation)
	void DisableMovement();
	void DisableMovement_Implementation();
	bool DisableMovement_Validate();
	UFUNCTION(Server, Reliable, WithValidation)
	void EnableMovement();
	void EnableMovement_Implementation();
	bool EnableMovement_Validate();	

	void Fire();
};

