// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BaseGamer.generated.h"

UCLASS()
class LD33_API ABaseGamer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseGamer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseArmor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseMeleeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseMagicAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float BaseHealPower;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float Health;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float Armor;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float MeleeAttack;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float MagicAttack;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float HealPower;
	
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Respawn();

	UPROPERTY(BlueprintReadOnly, Category = Info)
	class AGuild* Guild;

	UPROPERTY(BlueprintReadOnly, Category = Info)
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = Info)
	bool IsMale;

	UPROPERTY(BlueprintReadOnly, Category = Info)
	bool IsLeader;


};
