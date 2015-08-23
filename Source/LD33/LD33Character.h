// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "LD33Character.generated.h"

UCLASS(Blueprintable)
class ALD33Character : public ACharacter
{
	GENERATED_BODY()

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	

public:
	ALD33Character();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void FrontalConeAttack(FVector targetPt);
	void SoulDrainAttack(AActor* target);
	void MeteorAttack(FVector targetPt);

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = Stats)
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float MaxMana;

	UPROPERTY(BlueprintReadOnly, Category = Ability)
	float AbilityCooldown;

	UPROPERTY(EditAnywhere, Category = Spawn)
	TSubclassOf<class AMeteorProjectile> MeteorProjectileType;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent, Category = Death)
	void OnDeath();

	bool UseMana(float qty);

	UFUNCTION(BlueprintImplementableEvent, Category = Death)
	void OnConeAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = Death)
	void OnSoulDrain(class AActor* target);

};

