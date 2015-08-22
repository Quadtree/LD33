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
	
	MaxHealth = BaseMaxHealth;
	Health = MaxHealth;
	Armor = BaseArmor;
	MeleeAttack = BaseMeleeAttack;
	MagicAttack = BaseMagicAttack;
	HealPower = BaseHealPower;

	TArray<FString> syl;

	FFileHelper::LoadANSITextFileToStrings(*(FPaths::GameContentDir() + "Data/Syllables.txt"), nullptr, syl);

	while (true)
	{

	}
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

float ABaseGamer::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage > 0)
	{
		Damage = Damage * (1 - Armor);
		Health -= Damage;

		if (Health <= 0)
		{
			Respawn();
		}
	}
	else
	{
		// prevent overhealing
		Health = FMath::Min(Health - Damage, MaxHealth);
	}

	return Damage;
}

void ABaseGamer::Respawn()
{
	Health = MaxHealth;

	SetActorLocation(FMath::RandPointInBox(FBox(FVector(-500, -500, -1000), FVector(500, 500, -1000))));
}