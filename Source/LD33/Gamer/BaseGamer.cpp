// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "BaseGamer.h"


// Sets default values
ABaseGamer::ABaseGamer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseGamer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGamer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ABaseGamer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

