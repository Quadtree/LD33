// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD33.h"
#include "LD33GameMode.h"
#include "LD33PlayerController.h"
#include "LD33Character.h"
#include "GenericMob.h"
#include "Landscape.h"
#include "EngineUtils.h"

ALD33GameMode::ALD33GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ALD33PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	MinMobs = 32;
}

void ALD33GameMode::BeginPlay()
{
	Super::BeginPlay();

	RespawnMobs();

	FTimerHandle h1;

	GetWorld()->GetTimerManager().SetTimer(h1, this, &ALD33GameMode::RespawnMobs, 5.f, true, FMath::FRandRange(0, 5.f));
}

void ALD33GameMode::RespawnMobs()
{
	if (!MobTypes.Num()) return;

	int32 count = 0;

	for (TActorIterator<AGenericMob> i(GetWorld()); i; ++i)
	{
		count++;
	}

	while (count < MinMobs)
	{
		auto m = GetWorld()->SpawnActor<AGenericMob>(MobTypes[FMath::Rand() % MobTypes.Num()], GetPointOutsideOfTown(), FRotator::ZeroRotator);
		if(m) m->SpawnDefaultController();

		++count;
	}
}

bool ALD33GameMode::GetTerrainHeightAt(FVector2D pt, FVector& outHeight)
{
	FHitResult hit;
	if (GetWorld()->LineTraceSingleByChannel(hit, FVector(pt.X, pt.Y, 5000), FVector(pt.X, pt.Y, -5000), ECollisionChannel::ECC_Visibility))
	{
		if (hit.Actor.IsValid() && Cast<ALandscape>(hit.Actor.Get()))
		{
			outHeight = hit.ImpactPoint;
			return true;
		}
	}

	return false;
}

FVector ALD33GameMode::GetPointOutsideOfTown()
{
	FVector pt;

	for (int32 i = 0; i < 10000; ++i)
	{
		if (GetTerrainHeightAt(FVector2D(FMath::RandRange(-WORLD_SIZE, WORLD_SIZE), FMath::RandRange(-WORLD_SIZE, WORLD_SIZE)), pt))
		{
			break;
		}
	}

	return pt;
}