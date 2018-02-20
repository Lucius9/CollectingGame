// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectingHud.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"

ACollectingHud::ACollectingHud()
{

}

void ACollectingHud::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX / 2, Canvas->ClipY / 2);

	float PosX = Center.X;
	float PosY = Center.Y + 20.0f;

	const FVector2D CrosshairDrawPosition(PosX, PosY);

	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	
	Canvas->DrawItem(TileItem);
}


