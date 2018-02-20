// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CollectingGameState.generated.h"

/**
 * 
 */
UCLASS()
class COLLECTINGGAME_API ACollectingGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int MaxCollectingScore = 3;

public:
	ACollectingGameState();		
	bool CheckCollectingVictory(class ACollectingGameCharacter *Character);

	DECLARE_EVENT(ACollectingGameState,FOnCollectingVictory)
	FOnCollectingVictory& OnCollectingVictory() { return CollectingVictoryEvent; }

	DECLARE_EVENT_OneParam(ACollectingGameState,FOnCollectingSetWon,ACollectingGameCharacter*)
	FOnCollectingSetWon& OnCollectingSetWon() { return CollectingSetWonEvent; }

private:
	FOnCollectingVictory CollectingVictoryEvent;
	FOnCollectingSetWon CollectingSetWonEvent;

public:
	void EndGame();
	void EndSet(ACollectingGameCharacter* Character);
};
