// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "CollectingPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class COLLECTINGGAME_API ACollectingPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Score")
		int CollectingScore;

public:
	virtual void PostInitializeComponents();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	void IncreaseCollectingScore(int AdditionalScore);
	int GetCollectingScore();
	
};
