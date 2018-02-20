// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CollectingGameCharacter.h"
#include "CollectingGameGameMode.generated.h"

UCLASS(minimalapi)
class ACollectingGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collecting Game Parameters", meta = (AllowPrivateAccess = "true"))
	int MaxCollectingScore = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collecting Game Parameters", meta = (AllowPrivateAccess = "true"))
	int MaxTime = 20;	
	
public:
	ACollectingGameGameMode();	
	virtual void BeginPlay();		
	void DisableAllPlayers();
	virtual AActor* ChoosePlayerStart_Implementation(AController *Player) override;		
	void RespawnAllPlayers();
	AActor* FindSpawnActor(AController* Controller);	
	void OnEndSet(ACollectingGameCharacter* Character);
	void OnEndGame();
};