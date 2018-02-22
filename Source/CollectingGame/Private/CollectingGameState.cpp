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
	ACollectingPlayerState* CPS = Cast<ACollectingPlayerState>(Character->PlayerState);
	if (CPS)
	{			
		if (CPS->GetCollectingScore() >= MaxCollectingScore)
		{			
			return true;
		}		
	}	
	return false;	
}

void ACollectingGameState::EndGame()
{
	
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


