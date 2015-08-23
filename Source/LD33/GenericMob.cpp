// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "GenericMob.h"
#include "Gamer/BaseGamer.h"
#include "LD33Character.h"
#include "AIController.h"


// Sets default values
AGenericMob::AGenericMob()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenericMob::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;

	FTimerHandle h2;
	GetWorld()->GetTimerManager().SetTimer(h2, this, &AGenericMob::ScanArea, 1.f, true, FMath::FRandRange(0, 1.f));
}

// Called every frame
void AGenericMob::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (GetActorLocation().SizeSquared2D() < FMath::Square(3500)) Destroy();
}

// Called to bind functionality to input
void AGenericMob::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AGenericMob::ScanArea()
{
	IsCurrentlyAttacking = false;

	TArray<FOverlapResult> res;

	int32 numGuildMembersNearby = 0;

	if (!AggroedOn)
	{
		if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(1000)))
		{
			for (auto a : res)
			{
				if (auto g = Cast<ABaseGamer>(a.Actor.Get()))
				{
					AggroedOn = g;
					InitialAggroPoint = GetActorLocation();
				}
			}
		}
	}
	else
	{
		if (auto t = Cast<ABaseGamer>(AggroedOn))
		{
			if (t->Health <= 0)
			{
				AggroedOn = nullptr;
				return;
			}
		}

		auto c = Cast<AAIController>(GetController());

		if (c && c->IsValidLowLevel() && !c->IsPendingKill() && AggroedOn && AggroedOn->IsValidLowLevel() && !AggroedOn->IsPendingKill())
		{
			if (c->MoveToActor(AggroedOn, 150) == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				AggroedOn->TakeDamage(MeleeAttack, FDamageEvent(), GetController(), this);
				IsCurrentlyAttacking = true;
			}
		}

		if (FVector::DistSquared(InitialAggroPoint, AggroedOn->GetActorLocation()) > FMath::Square(8000))
		{
			AggroedOn = nullptr;
		}
	}
}

float AGenericMob::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Cast<ALD33Character>(DamageCauser))
	{
		return 0;
	}

	Health -= Damage;

	if (Health <= 0)
	{
		OnDeath();
		Destroy();
	}

	if (Cast<ABaseGamer>(DamageCauser) && !AggroedOn)
	{
		AggroedOn = DamageCauser;
		InitialAggroPoint = GetActorLocation();
	}

	return Damage;
}