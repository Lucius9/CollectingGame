// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectingPlayerController.h"

void ACollectingPlayerController::DisablePlayer()
{
	this->GetPawn()->DisableInput(this);
}


