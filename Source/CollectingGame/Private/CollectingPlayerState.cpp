// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectingPlayerState.h"
#include <string>


void ACollectingPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollectingScore = 0;
}

void ACollectingPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(ACollectingPlayerState, CollectingScore);
}

void ACollectingPlayerState::IncreaseCollectingScore(int AdditionalScore)
{
	CollectingScore += AdditionalScore;
	std::string s = std::to_string(CollectingScore);	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, UTF8_TO_TCHAR(s.c_str()));
	}
}

int ACollectingPlayerState::GetCollectingScore()
{
	return CollectingScore;
}



