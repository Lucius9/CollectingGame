// Fill out your copyright notice in the Description page of Project Settings.

#include "GoldenCube.h"


// Sets default values
AGoldenCube::AGoldenCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
	
	this->GetStaticMeshComponent()->SetSimulatePhysics(true);
	this->GetStaticMeshComponent()->bGenerateOverlapEvents = true;

	this->MinNetUpdateFrequency = 100.0f;
	this->bReplicates = true;
	this->bAlwaysRelevant = true;	
	this->bNetLoadOnClient = true;
}

void AGoldenCube::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoldenCube, isHeld);	
	DOREPLIFETIME(AGoldenCube, InitialPosition);
}

// Called when the game starts or when spawned
void AGoldenCube::BeginPlay()
{
	Super::BeginPlay();
	InitialPosition = this->GetActorLocation();
}

// Called every frame
void AGoldenCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoldenCube::SetIsHeld(bool newIsHeld)
{
	isHeld = newIsHeld;
}

bool AGoldenCube::GetIsHeld()
{
	return isHeld;
}

void AGoldenCube::SetLastHolder(APawn* Pawn)
{
	LastHolder = Pawn;
}

APawn* AGoldenCube::GetLastHolder()
{
	return LastHolder;
}

void AGoldenCube::ResetLocation()
{
	SetActorLocation(InitialPosition);
}

