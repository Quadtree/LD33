// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "LD33GameMode.generated.h"

UCLASS(minimalapi)
class ALD33GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALD33GameMode();

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Mob)
	TArray<TSubclassOf<class AGenericMob>> MobTypes;

	UPROPERTY(EditAnywhere, Category = Mob)
	int32 MinMobs;

	void RespawnMobs();

	bool GetTerrainHeightAt(FVector2D pt, FVector& outHeight);
	FVector GetPointOutsideOfTown();
};



