// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD33.h"
#include "LD33Character.h"
#include "MeteorProjectile.h"
#include "Gamer/BaseGamer.h"

ALD33Character::ALD33Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MaxHealth = 400000;
}

void ALD33Character::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	
}


void ALD33Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AbilityCooldown -= DeltaSeconds;
}

void ALD33Character::FrontalConeAttack(FVector targetPt)
{
	if (AbilityCooldown > 0) return;

	UE_LOG(LogTemp, Display, TEXT("Frontal cone attack at %s"), *targetPt.ToString());

	FVector deltaToPt = targetPt - GetActorLocation();
	deltaToPt.Z = 0;
	deltaToPt.Normalize();

	SetActorRotation(deltaToPt.Rotation());

	TArray<FOverlapResult> res;

	if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(1500)))
	{
		for (auto a : res)
		{
			if (a.Actor.IsValid() && Cast<ABaseGamer>(a.Actor.Get()))
			{
				FVector delta = a.Actor->GetActorLocation() - GetActorLocation();
				delta.Normalize();

				FVector facing = GetActorRotation().RotateVector(FVector::RightVector);

				float ang = FVector::DotProduct(delta, facing);
				float ang2 = FVector::DotProduct(delta, GetActorRotation().RotateVector(FVector::ForwardVector));

				UE_LOG(LogTemp, Display, TEXT("%s -> %s %s"), *a.Actor->GetName(), *FString::SanitizeFloat(ang), *FString::SanitizeFloat(ang2));

				if (FMath::Abs(ang) < 0.3f && ang2 > 0)
				{
					a.Actor->TakeDamage(2000, FDamageEvent(), GetController(), this);
				}
			}
		}
	}

	AbilityCooldown = 1;
}

void ALD33Character::SoulDrainAttack(AActor* target)
{
	if (AbilityCooldown > 0) return;

	if (target)
	{
		UE_LOG(LogTemp, Display, TEXT("SoulDrainAttack at %s"), *target->GetName());

		float initialHp = 100000;

		if (auto a = Cast<ABaseGamer>(target)) initialHp = a->Health;

		float actualDamage = target->TakeDamage(3500, FDamageEvent(), GetController(), this);

		if (actualDamage >= initialHp)
		{
			Health += 40000;
			UE_LOG(LogTemp, Display, TEXT("Successful soul drain"));
		}

		AbilityCooldown = 1;
	}
}

void ALD33Character::MeteorAttack(FVector targetPt)
{
	if (AbilityCooldown > 0) return;

	UE_LOG(LogTemp, Display, TEXT("MeteorAttack at %s"), *targetPt.ToString());

	AMeteorProjectile* prj = GetWorld()->SpawnActor<AMeteorProjectile>(MeteorProjectileType, targetPt + FVector(0, 0, 5000), FRotator::ZeroRotator);
	
	if (prj)
	{
		UPrimitiveComponent* p = Cast<UPrimitiveComponent>(prj->GetRootComponent());
		if (p)
		{
			p->SetPhysicsLinearVelocity(FVector(0, 0, -1800));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn meteor"));
	}

	AbilityCooldown = 1;
}