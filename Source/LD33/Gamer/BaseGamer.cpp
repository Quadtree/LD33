// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "BaseGamer.h"
#include "LD33Character.h"
#include "GamerMessage.h"


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

	FFileHelper::LoadANSITextFileToStrings(*(FPaths::GameContentDir() + "Data/NameSyllables.txt"), nullptr, syl);
	
	if (syl.Num())
	{
		while (true)
		{
			FString newName = "";

			while (FMath::RandRange(1, 4) != 1)
			{
				newName += syl[FMath::Rand() % syl.Num()];
			}

			if (newName.Len() > 9 || newName.Len() < 4) continue;

			bool isMaleName = !newName.EndsWith("A") && newName.EndsWith("E");

			if (isMaleName != IsMale) continue;

			GamerName = newName.Left(1) + newName.Right(newName.Len() - 1).ToLower();
			break;
		}

		UE_LOG(LogLD33, Display, TEXT("Name: %s"), *GamerName);
	}

	BlueprintInit();

	FTimerHandle h1, h2;

	GetWorld()->GetTimerManager().SetTimer(h1, this, &ABaseGamer::UpdateMessageQueue, 0.5f, true, FMath::FRandRange(0, 0.5f));
	GetWorld()->GetTimerManager().SetTimer(h2, this, &ABaseGamer::UpdateState, 1.f, true, FMath::FRandRange(0, 1.f));
}


void ABaseGamer::UpdateMessageQueue()
{

}

void ABaseGamer::UpdateState()
{
	TArray<FOverlapResult> res;

	if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(4000)))
	{
		for (auto a : res)
		{
			if (CurrentState == GamerState::GS_Farming || CurrentState == GamerState::GS_Scouting || CurrentState == GamerState::GS_IdleInTown || CurrentState == GamerState::GS_AttackingBoss)
			{
				if (ALD33Character* t = Cast<ALD33Character>(a.Actor.Get()))
				{
					if (CurrentState == GamerState::GS_Farming || CurrentState == GamerState::GS_Scouting)
					{
						CurrentState = GamerState::GS_ReportingBossSighting;
						return;
					}
					if (CurrentState == GamerState::GS_AttackingBoss && IsLeader)
					{
						// send message indicating attack
						return;
					}
					if (CurrentState == GamerState::GS_IdleInTown)
					{
						CurrentState = GamerState::GS_AttackingBoss;
						return;
					}
				}
			}
		}
	}
}

void ABaseGamer::SendGamerMessage(GamerMessageType type)
{
	FGamerMessage msg;
	msg.Type = type;
	msg.Sender = this;

	TArray<FOverlapResult> res;
	if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(4000)))
	{
		for (auto a : res)
		{
			if (a.Actor.Get() != this)
			{
				if (ABaseGamer* t = Cast<ABaseGamer>(a.Actor.Get()))
				{
					t->ReceiveGamerMessage(msg);
				}
			}
		}
	}
}

void ABaseGamer::ReceiveGamerMessage(const FGamerMessage& msg)
{
	if (CurrentState != GamerState::GS_AttackingBoss && CurrentState != GamerState::GS_PlayerVersusPlayer && msg.Type == GamerMessageType::GMT_RequestMemberJoin && msg.Sender->Guild == this->Guild && msg.Sender->IsLeader)
	{
		CurrentState = GamerState::GS_FollowingLeader;
	}

	if (CurrentState != GamerState::GS_AttackingBoss && CurrentState != GamerState::GS_PlayerVersusPlayer && msg.Type == GamerMessageType::GMT_ReportBossUp && msg.Sender->Guild == this->Guild && IsLeader)
	{
		CurrentState = GamerState::GS_LookingForMore;
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