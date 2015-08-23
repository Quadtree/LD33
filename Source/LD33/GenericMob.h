// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "GenericMob.generated.h"

UCLASS()
class LD33_API AGenericMob : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGenericMob();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, Category = Health)
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = Health)
	float MeleeAttack;

	float Health;
	
	void ScanArea();

	FVector InitialAggroPoint;

	UPROPERTY()
	AActor* AggroedOn;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent, Category = Death)
	void OnDeath();

	UPROPERTY(BlueprintReadOnly, Category = Attack)
	bool IsCurrentlyAttacking;
};
