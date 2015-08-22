// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Guild.generated.h"

UCLASS()
class LD33_API AGuild : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGuild();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	bool GetTerrainHeightAt(FVector2D pt, FVector& outHeight);
	
	FVector GetPointOutsideOfTown();
	FVector GetPointInTown();

	UPROPERTY(BlueprintReadOnly, Category = Name)
	FString GuildName;

	int32 NumMembers;

	UPROPERTY(EditAnywhere, Category = Spawn)
	TArray<TSubclassOf<class ABaseGamer>> GamerTypes;
};
