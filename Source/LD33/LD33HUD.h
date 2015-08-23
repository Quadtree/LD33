// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "LD33HUD.generated.h"

/**
 * 
 */
UCLASS()
class LD33_API ALD33HUD : public AHUD
{
	GENERATED_BODY()

	void DrawHUD();
	
	UPROPERTY(EditAnywhere, Category = Font)
	class UFont* NametagFont;

	UPROPERTY(EditAnywhere, Category = Font)
	class UFont* ChatMessageFont;

	UPROPERTY(EditAnywhere, Category = Font)
	UTexture2D* MainUITexture;

	UPROPERTY(EditAnywhere, Category = Font)
	TArray<UTexture2D*> AbilityTextures;

	UPROPERTY(EditAnywhere, Category = Font)
	TArray<FString> AbilityText;
public:
	void AddChatMessage(FVector pos, FString text);

private:
	TArray<FVector> ChatMessagePos;
	TArray<float> ChatMessageTimeLeft;
	TArray<FString> ChatMessageText;
};
