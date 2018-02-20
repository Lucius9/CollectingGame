// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CollectingHud.generated.h"

/**
 * 
 */
UCLASS()
class COLLECTINGGAME_API ACollectingHud : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UTexture2D* CrosshairTexture;
	
public:
	ACollectingHud();

	virtual void DrawHUD() override;	
};
