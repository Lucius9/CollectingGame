// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GoldenPlatform.generated.h"

UCLASS()
class COLLECTINGGAME_API AGoldenPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;
	
public:	
	// Sets default values for this actor's properties
	AGoldenPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override; 	

	UFUNCTION()
	virtual void OnHit(class AActor* Actor, class AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void CubeOverlapped(class AGoldenCube* Actor);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCubeOverlapped(class AGoldenCube* Actor);
	virtual void ServerCubeOverlapped_Implementation(class AGoldenCube* Actor);
	virtual bool ServerCubeOverlapped_Validate(class AGoldenCube* Actor);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
