// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CollectingGameGameMode.h"
#include "CollectingGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CollectingPlayerState.h"
#include "CollectingGameState.h"
#include "CollectingPlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Engine.h"

ACollectingGameGameMode::ACollectingGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	ACollectingGameState* GS = Cast<ACollectingGameState>(GameState);
	
}

void ACollectingGameGameMode::BeginPlay()
{
	Super::BeginPlay();
	ACollectingGameState* GS = Cast<ACollectingGameState>(GameState);
	if (GS)
	{		
		GS->OnCollectingVictory().AddUObject(this, &ACollectingGameGameMode::OnEndGame);
		FString test = "BindVictory " + FString::FromInt(GS->OnCollectingVictory().IsBoundToObject(this));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, test);

		GS->OnCollectingSetWon().AddUObject(this, &ACollectingGameGameMode::OnEndSet);
		FString tmp = "BindSetWon " + FString::FromInt(GS->OnCollectingVictory().IsBoundToObject(this));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, tmp);	
	}
}

void ACollectingGameGameMode::DisableAllPlayers()
{
	UWorld* World = GetWorld();
	
	if (World)
	{
		TActorIterator<ACollectingGameCharacter>ActorItr(World);
		for (ActorItr; ActorItr; ++ActorItr)
		{
			ActorItr->DisableMovement();
		}	
	}
}

AActor* ACollectingGameGameMode::ChoosePlayerStart_Implementation(AController *Player)
{
	UWorld *World = GetWorld();
	
	if (World)
	{
		TActorIterator<APlayerStart> PSItr(World);	
		int ID=Player->PlayerState->PlayerId;
		FString Tag = FString::FromInt(ID);
		for (PSItr; PSItr; ++PSItr)
		{
			if (PSItr->PlayerStartTag == FName(TEXT("")))
			{
				APlayerStart* PS = *PSItr;	
				PS->SetReplicates(true);
				PS->bAlwaysRelevant = true;
				PS->bNetLoadOnClient = true;
				
				PS->PlayerStartTag = FName(*Tag);				
				return PS;
			}
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ACollectingGameGameMode::RespawnAllPlayers()
{
	UWorld* World = GetWorld();
	if (World)
	{
		int i = 1;
		FConstPlayerControllerIterator PCItr = World->GetPlayerControllerIterator();
		for (PCItr; PCItr; ++PCItr)
		{			

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::FromInt(i));
 			APlayerController* PC = PCItr->Get(); 			
 			ACollectingGameCharacter* Character = Cast<ACollectingGameCharacter>(PC->GetCharacter());
			AActor* PlayerStart = FindSpawnActor(PC);			
 			if (Character && PlayerStart)
			{				
 				ACollectingGameCharacter* Pawn=Cast<ACollectingGameCharacter>(SpawnDefaultPawnFor(PC, PlayerStart));
				if (Pawn)
				{					
					PC->Possess(Pawn);								
					Character->Destroy();
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, Pawn->GetControlRotation().ToString());
				}			
			}			
			
		}
	}	
}

AActor* ACollectingGameGameMode::FindSpawnActor(AController* Controller)
{
	int ID = Controller->PlayerState->PlayerId;
	FName NameID = FName(*FString::FromInt(ID));
	UWorld* World = GetWorld();
	if (World)
	{
		
		TActorIterator<APlayerStart> ActorItr(World);
		for (ActorItr; ActorItr; ++ActorItr)
		{
			APlayerStart* PlayerStart = *ActorItr;			
			if (ActorItr->PlayerStartTag == NameID)
			{
				return *ActorItr;
			}
		}
	}
	return NULL;
}

void ACollectingGameGameMode::OnEndSet(ACollectingGameCharacter *Character)
{	
	RespawnAllPlayers();
}

void ACollectingGameGameMode::OnEndGame()
{		
	DisableAllPlayers();	
	UWorld* World = GetWorld();
	this->ProcessServerTravel("/Game/Maps/Level2?listen", true);
	/*if (World)
	{		
		World->ServerTravel("/Game/Maps/Level2?listen", true, true);
	}	*/	
}