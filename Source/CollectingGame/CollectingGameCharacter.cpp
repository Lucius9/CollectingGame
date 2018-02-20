// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CollectingGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GoldenCube.h"
#include "CollectingPlayerState.h"
#include "Engine.h"

#define print(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}

//////////////////////////////////////////////////////////////////////////
// ACollectingGameCharacter

ACollectingGameCharacter::ACollectingGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	//Set CurrentCamera to Third Person
	CurrentCamera = FollowCamera;	
	CurrentCamera->SetActive(true);
	
	
	 //Create FPS Camera
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	//FPSCamera->SetupAttachment(GetMesh());
	FPSCamera->SetRelativeLocation(FVector(0.0f, 10.0f, 0.0f));
	FPSCamera->SetupAttachment(GetMesh(),FName(TEXT("FPSCam")));
	FPSCamera->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));	
	FPSCamera->SetActive(false);
	FPSCamera->bUsePawnControlRotation = true;

	HandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("HandleComponent"));		
	HandleComponent->bSoftLinearConstraint = false;

	HeldObjectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectLocation"));
	HeldObjectLocation->SetupAttachment(GetMesh());
	HeldObjectLocation->SetRelativeLocation(FVector(0.0f,150.0f,150.0f));	

	bReplicates = true;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	GetCharacterMovement()->MaxWalkSpeed = 375.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 100.0f;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));	
	WeaponMesh->SetupAttachment(GetMesh(), FName(TEXT("WeaponHolder")));

	ProjectileSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnLocation"));
	ProjectileSpawnLocation->SetupAttachment(WeaponMesh);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACollectingGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACollectingGameCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACollectingGameCharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACollectingGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACollectingGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &ACollectingGameCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACollectingGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ACollectingGameCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACollectingGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACollectingGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACollectingGameCharacter::TouchStopped);

	//Crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACollectingGameCharacter::OnCrouchPressed);

	//Pick Up and Drop Objects
	PlayerInputComponent->BindAction("PickUp",IE_Pressed, this, &ACollectingGameCharacter::OnPickUpKeyPressed);

	//Switch Active Camera
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ACollectingGameCharacter::SwitchCamera);
	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACollectingGameCharacter::OnResetVR);	
}

void ACollectingGameCharacter::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	if (HandleComponent->GetGrabbedComponent() != NULL)
	{
		HandleComponent->SetTargetLocation(HeldObjectLocation->GetComponentLocation());		
	}
	
}

void ACollectingGameCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectingGameCharacter, holdingObject);
	DOREPLIFETIME(ACollectingGameCharacter, HeldObjectLocation);
	DOREPLIFETIME(ACollectingGameCharacter, Score);
	DOREPLIFETIME(ACollectingGameCharacter, bCanMove);
	DOREPLIFETIME(ACollectingGameCharacter, bJumpPressed);
	DOREPLIFETIME(ACollectingGameCharacter, bCrouchPressed);
}


void ACollectingGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACollectingGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ACollectingGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ACollectingGameCharacter::TurnAtRate(float Rate)
{	
	if (bCameraMovement)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACollectingGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (bCameraMovement)
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACollectingGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && bCanMove)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACollectingGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) && bCanMove)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACollectingGameCharacter::AddControllerYawInput(float val)
{
	if (bCameraMovement)
	{
		APawn::AddControllerYawInput(val);
	}
}

void ACollectingGameCharacter::AddControllerPitchInput(float val)
{
	if (bCameraMovement)
	{
		APawn::AddControllerPitchInput(val);
	}
}

void ACollectingGameCharacter::Jump()
{
	if (!bCrouchPressed && bCanMove)
	{
		if (GetCharacterMovement()->Velocity.Size() > 0.0f)
		{
			GetCharacterMovement()->JumpZVelocity = 365.0f;
			Super::Jump();
		}
		else
		{
			GetCharacterMovement()->JumpZVelocity = 340.0f;
			Super::Jump();
		}
		bJumpPressed = true;
	}
}


void ACollectingGameCharacter::StopJumping()
{
	bJumpPressed = false;
}

void ACollectingGameCharacter::OnCrouchPressed()
{
	if (bCrouchPressed)
	{
		bCrouchPressed = false;
	}
	else
	{
		bCrouchPressed = true;
	}

	if (bCrouchPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = 160.0f;
	}
	else
	{		
		GetCharacterMovement()->MaxWalkSpeed = 375.0f;		
	}
}

void ACollectingGameCharacter::OnPickUpKeyPressed()
{
	if (IsHoldingObject())
	{
		DropObject();
	}
	else
	{
		PickUpObject();
	}
}


void ACollectingGameCharacter::PickUpObject()
{
	if (Role <= ROLE_Authority)
	{
		ServerPickUpObject();
	}
}

void ACollectingGameCharacter::ServerPickUpObject_Implementation()
{
	UWorld *World = GetWorld();
	if (World)
	{
		const FName TraceTag("HoldingTrace");
		World->DebugDrawTraceTag = TraceTag;

		FCollisionQueryParams HoldingTraceQuery = FCollisionQueryParams(TraceTag, false, this);
		FHitResult HoldingTraceResult(ForceInit);
		FVector Start = CurrentCamera->GetComponentLocation();
		FVector End = (CurrentCamera->GetForwardVector() * 600) + Start;

		bool blocking = World->LineTraceSingleByChannel(HoldingTraceResult, Start, End, ECollisionChannel::ECC_Visibility, HoldingTraceQuery);

		if (blocking && HoldingTraceResult.Component->IsSimulatingPhysics())
		{
			UPrimitiveComponent *ObjectToPick = HoldingTraceResult.GetComponent();
			AActor* SomeActor = ObjectToPick->GetOwner();
			AGoldenCube* GoldenCube = Cast<AGoldenCube>(SomeActor);

			if (GoldenCube && !GoldenCube->GetIsHeld())
			{
				HandleComponent->GrabComponentAtLocation(ObjectToPick, NAME_None, ObjectToPick->GetComponentLocation());
				HandleComponent->GetGrabbedComponent()->SetEnableGravity(false);
				holdingObject = true;
				GoldenCube->SetIsHeld(true);
				GoldenCube->SetLastHolder(this);
			}				
		}

	}
}

bool ACollectingGameCharacter::ServerPickUpObject_Validate()
{
	return true;
}

void ACollectingGameCharacter::DropObject()
{
	if (Role <= ROLE_Authority)
	{
		ServerDropObject();
	}
}

void ACollectingGameCharacter::ServerDropObject_Implementation()
{
	
	UPrimitiveComponent *HeldObject = HandleComponent->GrabbedComponent;
	AActor* SomeActor = HeldObject->GetOwner();
	AGoldenCube* GoldenCube = Cast<AGoldenCube>(SomeActor);
	if (GoldenCube)
	{
		HeldObject->SetEnableGravity(true);
		HandleComponent->ReleaseComponent();
		holdingObject = false;
		GoldenCube->SetIsHeld(false);
	}	
}

bool ACollectingGameCharacter::ServerDropObject_Validate()
{
	return true;
}

void ACollectingGameCharacter::SwitchCamera()
{	
	AController *Controller = Cast<AController>(GetController());
	
	CurrentCamera->Deactivate();

	if (Controller)
	{
		
		FRotator ControlRotator = Controller->GetControlRotation();
		this->SetActorRotation(FRotator(0.0f, ControlRotator.Yaw, 0.0f));
		if (CurrentCamera == FPSCamera)
		{
			this->bUseControllerRotationYaw = false;
			CurrentCamera = FollowCamera;
		}
		else
		{
			this->bUseControllerRotationYaw = true;
			CurrentCamera = FPSCamera;				
		}
	}
	CurrentCamera->Activate();	
}

void ACollectingGameCharacter::IncreaseCollectingScore(int AdditionalScore)
{
	if (Role <= ROLE_Authority)
	{
		ServerIncreaseCollectingScore(AdditionalScore);
	}
}

void ACollectingGameCharacter::ServerIncreaseCollectingScore_Implementation(int AdditionalScore)
{
	ACollectingPlayerState* MyPlayerState = Cast<ACollectingPlayerState>(this->PlayerState);

	if (MyPlayerState)
	{
		MyPlayerState->IncreaseCollectingScore(AdditionalScore);
	}
}

bool ACollectingGameCharacter::ServerIncreaseCollectingScore_Validate(int AdditionalScore)
{
	return true;
}

void ACollectingGameCharacter::DisableMovement_Implementation()
{
	bCanMove = false;
	bCameraMovement = false;	
}

bool ACollectingGameCharacter::DisableMovement_Validate()
{
	return true;
}

void ACollectingGameCharacter::EnableMovement_Implementation()
{
	bCanMove = true;	
	bCameraMovement = true;	
}

bool ACollectingGameCharacter::EnableMovement_Validate()
{
	return true;
}

void ACollectingGameCharacter::ResetCameraRotation()
{
	if (CurrentCamera == FPSCamera)
	{
		FPSCamera->SetRelativeRotation(FRotator(10.0f, 90.0f, -90.0f));
	}
	else
	{
		CameraBoom->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
}