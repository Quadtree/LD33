// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "Landscape.h"
#include "Guild.h"
#include "Gamer/BaseGamer.h"


// Sets default values
AGuild::AGuild()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGuild::BeginPlay()
{
	Super::BeginPlay();

	TArray<FString> nouns;
	TArray<FString> ofs;
	TArray<FString> adjectives;

	FFileHelper::LoadANSITextFileToStrings(*(FPaths::GameContentDir() + "Data/Noun.txt"), nullptr, nouns);
	FFileHelper::LoadANSITextFileToStrings(*(FPaths::GameContentDir() + "Data/Of.txt"), nullptr, ofs);
	FFileHelper::LoadANSITextFileToStrings(*(FPaths::GameContentDir() + "Data/Adjective.txt"), nullptr, adjectives);

	GuildName = "";
	int32 idx;

	while (!GuildName.FindChar(' ', idx))
	{
		GuildName = "";

		if (FMath::RandBool()) GuildName += adjectives[FMath::Rand() % adjectives.Num()] + " ";
		GuildName += nouns[FMath::Rand() % nouns.Num()];
		if (FMath::RandBool()) GuildName += " of " + ofs[FMath::Rand() % ofs.Num()];
	}
	
	int32 memberCount = FMath::RandRange(3, 10);

	bool startsAssembled = FMath::RandRange(1, memberCount) == 1;

	TArray<FVector> spawnPoints;

	if (startsAssembled)
	{
		FVector spawnPoint = GetPointOutsideOfTown();

		for (int32 i = 0; i < memberCount; ++i)
		{
			FVector psp;

			if (GetTerrainHeightAt(FVector2D(FMath::FRandRange(spawnPoint.X - 500, spawnPoint.X + 500), FMath::FRandRange(spawnPoint.Y - 500, spawnPoint.Y + 500)), psp))
			{
				spawnPoints.Add(psp);
			}
		}
	}
	else 
	{
		for (int32 i = 0; i < memberCount; ++i)
		{
			if (FMath::RandBool())
			{
				spawnPoints.Add(GetPointOutsideOfTown());
			}
			else
			{
				spawnPoints.Add(GetPointInTown());
			}
		}
	}

	UE_LOG(LogLD33, Display, TEXT("New guild: %s"), *GuildName);

	bool isLeader = true;

	for (auto a : spawnPoints)
	{
		UE_LOG(LogLD33, Display, TEXT("Spawn point: %s"), *a.ToString());

		if (GamerTypes.Num() > 0)
		{
			ABaseGamer* g = GetWorld()->SpawnActor<ABaseGamer>(GamerTypes[FMath::Rand() % GamerTypes.Num()], a, FRotator::ZeroRotator);
			
			if (g)
			{
				g->Guild = this;
				g->IsMale = FMath::RandBool();
				g->IsLeader = isLeader;
				isLeader = false;
			}
		}
		else
		{
			UE_LOG(LogLD33, Warning, TEXT("Warning: No gamer types in %s"), *GetName());
		}
	}
}

// Called every frame
void AGuild::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AGuild::GetTerrainHeightAt(FVector2D pt, FVector& outHeight)
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

FVector AGuild::GetPointOutsideOfTown()
{
	FVector pt;

	for (int32 i = 0; i < 10000; ++i)
	{
		if (GetTerrainHeightAt(FVector2D(FMath::RandRange(-23000, 23000), FMath::RandRange(-23000, 23000)), pt))
		{
			break;
		}
	}

	return pt;
}

FVector AGuild::GetPointInTown()
{
	FVector pt;

	for (int32 i = 0; i < 10000; ++i)
	{
		if (GetTerrainHeightAt(FVector2D(FMath::RandRange(-2000, 2000), FMath::RandRange(-2000, 2000)), pt))
		{
			break;
		}
	}

	return pt;
}