// Fill out your copyright notice in the Description page of Project Settings.

#include "GoldenTarget.h"


// Sets default values
AGoldenTarget::AGoldenTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	this->bReplicates = true;
	this->bAlwaysRelevant = true;
	this->bNetLoadOnClient = true;
}

// Called when the game starts or when spawned
void AGoldenTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoldenTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UFUNCTION()
void AGoldenTarget::OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

