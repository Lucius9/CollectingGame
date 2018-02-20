// Fill out your copyright notice in the Description page of Project Settings.

#include "GoldenPlatform.h"
#include "GoldenCube.h"
#include "CollectingGameCharacter.h"
#include "CollectingGameState.h"
#include "CollectingPlayerState.h"
#include "CollectingGameGameMode.h"

// Sets default values
AGoldenPlatform::AGoldenPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGoldenPlatform::OnBeginOverlap);

	this->OnActorHit.AddDynamic(this, &AGoldenPlatform::OnHit);
}

// Called when the game starts or when spawned
void AGoldenPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoldenPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGoldenPlatform::OnHit(class AActor* Actor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{	
	
}

void AGoldenPlatform::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AGoldenCube* GoldenCube = Cast<AGoldenCube>(OtherActor);
	if(GoldenCube)
	{
		CubeOverlapped(GoldenCube);
	}	
}

void AGoldenPlatform::CubeOverlapped(AGoldenCube *Actor)
{
	if (Role <= ROLE_Authority)
	{
		ServerCubeOverlapped(Actor);
	}
	
}

void AGoldenPlatform::ServerCubeOverlapped_Implementation(AGoldenCube *Actor)
{
	UWorld *World = GetWorld();
	if (World)
	{
		ACollectingGameState* GS = World->GetGameState<ACollectingGameState>();
		if(GS)
		{
			APawn* Pawn = Actor->GetLastHolder();
			ACollectingGameCharacter* Character = Cast<ACollectingGameCharacter>(Pawn);
			Actor->ResetLocation();
			if (Character)
			{
				ACollectingPlayerState* CPS = Cast<ACollectingPlayerState>(Character->PlayerState);
				if (CPS)
				{
					CPS->IncreaseCollectingScore(1);
					GS->EndSet(Character);
				}				
			}
		}
	}
}

bool AGoldenPlatform::ServerCubeOverlapped_Validate(AGoldenCube *Actor)
{
	return true;
}