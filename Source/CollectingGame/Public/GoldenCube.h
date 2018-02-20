// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h" 
#include "GoldenCube.generated.h"

UCLASS()
class COLLECTINGGAME_API AGoldenCube : public AStaticMeshActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Collecting Game")
	bool isHeld;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Collecting Game")
	APawn *LastHolder = NULL;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FVector InitialPosition;
			
public:	
	// Sets default values for this actor's properties
	AGoldenCube();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetIsHeld(bool newIsHeld);
	bool GetIsHeld();
	void SetLastHolder(APawn* Pawn);
	APawn* GetLastHolder();
	void ResetLocation();

	
	
};
