// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectingGameState.h"
#include "CollectingGameCharacter.h"
#include "CollectingPlayerState.h"
#include "Engine.h"

ACollectingGameState::ACollectingGameState()
{
	
}

bool ACollectingGameState::CheckCollectingVictory(ACollectingGameCharacter* Character)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("here"));
	ACollectingPlayerState* CPS = Cast<ACollectingPlayerState>(Character->PlayerState);
	if (CPS)
	{
		FString Score = "Score : " + FString::FromInt(CPS->GetCollectingScore());
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, Score);
		if (CPS->GetCollectingScore() >= MaxCollectingScore)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("true"));
			return true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("false"));
	}
	
	
	return false;	
}

void ACollectingGameState::EndGame()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("End Game Game State"));
	}	
}

void ACollectingGameState::EndSet(ACollectingGameCharacter* Character)
{	
	bool victory = CheckCollectingVictory(Character);
	if (victory)
	{
		CollectingVictoryEvent.Broadcast();
	}
	else
	{
		CollectingSetWonEvent.Broadcast(Character);
	}
	
}


