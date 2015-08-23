// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "BaseGamer.h"
#include "LD33Character.h"
#include "GamerMessage.h"
#include "EngineUtils.h"
#include "LD33HUD.h"
#include "Guild.h"
#include "AIController.h"
#include "IceBolt.h"


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
	IsCurrentlyAttacking = false;

	TArray<FOverlapResult> res;

	int32 numGuildMembersNearby = 0;

	if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(4000)))
	{
		for (auto a : res)
		{
			if (CurrentState == GamerState::GS_Farming || CurrentState == GamerState::GS_Scouting || CurrentState == GamerState::GS_IdleInTown || CurrentState == GamerState::GS_AttackingBoss || CurrentState == GamerState::GS_ApproachingBoss)
			{
				if (ALD33Character* t = Cast<ALD33Character>(a.Actor.Get()))
				{
					if (CurrentState == GamerState::GS_Farming || CurrentState == GamerState::GS_Scouting)
					{
						CurrentState = GamerState::GS_ReportingBossSighting;
						SendGamerMessage(GamerMessageType::GMT_ReportBossUp);
					}
					else if (CurrentState != GamerState::GS_AttackingBoss && CurrentState == GamerState::GS_ApproachingBoss && IsLeader)
					{
						SendGamerMessage(GamerMessageType::GMT_AttackBossNow);
						CurrentState = GamerState::GS_AttackingBoss;
					}
					// saw the boss while in town. just attack!
					else if (CurrentState != GamerState::GS_AttackingBoss && CurrentState == GamerState::GS_IdleInTown)
					{
						SendGamerMessage(GamerMessageType::GMT_AttackBossNow);
						CurrentState = GamerState::GS_AttackingBoss;
					}
				}
			}

			if (ABaseGamer* t = Cast<ABaseGamer>(a.Actor.Get()))
			{
				if (t->Guild == Guild)
				{
					++numGuildMembersNearby;

					if (CurrentState == GamerState::GS_LookingForMore && t->CurrentState != GamerState::GS_FollowingLeader)
					{
						SendGamerMessage(GamerMessageType::GMT_RequestMemberJoin);
					}

					if (CurrentState == GamerState::GS_AttackingBoss && t->CurrentState != GamerState::GS_AttackingBoss && t->CurrentState != GamerState::GS_PlayerVersusPlayer)
					{
						SendGamerMessage(GamerMessageType::GMT_AttackBossNow);
					}
				}
			}
		}
	}

	if (CurrentState == GamerState::GS_ReportingBossSighting && IsLeader)
	{
		CurrentState = GamerState::GS_LookingForMore;
	}

	if (CurrentState == GamerState::GS_ReportingBossSighting || CurrentState == GamerState::GS_FollowingLeader)
	{
		ABaseGamer* leader = nullptr;

		for (TActorIterator<ABaseGamer> i(GetWorld()); i; ++i)
		{
			if (i->Guild == Guild && i->IsLeader)
			{
				leader = *i;
				break;
			}
		}

		if (leader)
		{
			if (FVector::DistSquared(leader->GetActorLocation(), GetActorLocation()) > FMath::Square(750))
			{
				GetWorld()->GetNavigationSystem()->SimpleMoveToActor(GetController(), leader);
			}
			else
			{
				if (leader->CurrentState != GamerState::GS_LookingForMore && leader->CurrentState != GamerState::GS_ApproachingBoss && leader->CurrentState != GamerState::GS_AttackingBoss)
				{
					SendGamerMessage(GamerMessageType::GMT_ReportBossUp);
				}
				else
				{
					CurrentState = GamerState::GS_FollowingLeader;
				}
			}
		}
	}

	if (CurrentState == GamerState::GS_LookingForMore)
	{
		UE_LOG(LogLD33, Display, TEXT("%s LFM"), *GetName());

		float nearestDist = FLT_MAX;
		ABaseGamer* nearest = nullptr;

		for (TActorIterator<ABaseGamer> i(GetWorld()); i; ++i)
		{
			if (i->Guild == Guild && i->Health > 0)
			{
				float dist = FVector::DistSquared(i->GetActorLocation(), GetActorLocation());

				if (dist > FMath::Square(2500) && dist < nearestDist)
				{
					nearestDist = dist;
					nearest = *i;
				}
			}
		}

		if (!nearest)
		{
			UE_LOG(LogLD33, Display, TEXT("%s Starting to approach the boss"), *GetName());
			CurrentState = GamerState::GS_ApproachingBoss;
		}
		else
		{
			GetWorld()->GetNavigationSystem()->SimpleMoveToActor(GetController(), nearest);
		}
	}

	if (CurrentState == GamerState::GS_ApproachingBoss)
	{
		
		for (TActorIterator<ALD33Character> i(GetWorld()); i; ++i)
		{ 
			if (auto c = Cast<AAIController>(GetController()))
			{
				auto t = c->MoveToActor(*i, 1000);
				UE_LOG(LogLD33, Display, TEXT("%s approaching %s (%s)"), *GetName(), *i->GetName(), *FString::FromInt((int32)t));
			}
		}
	}

	if (CurrentState == GamerState::GS_AttackingBoss)
	{
		for (TActorIterator<ALD33Character> i(GetWorld()); i; ++i)
		{
			if (MeleeAttack > HealPower || MagicAttack > HealPower) Attack(*i);

			if (i->Health <= 0)
			{
				CurrentState = GamerState::GS_PlayerVersusPlayer;
			}
		}
	}
}

void ABaseGamer::SendGamerMessage(GamerMessageType type)
{
	UE_LOG(LogLD33, Display, TEXT("%s (%s) send %s"), *GetName(), *FString::FromInt(IsLeader), *FString::FromInt((int32)type));

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

	for (TActorIterator<ALD33HUD> i(GetWorld()); i; ++i)
	{
		i->AddChatMessage(GetActorLocation(), msg.ToString());
	}
}

void ABaseGamer::ReceiveGamerMessage(const FGamerMessage& msg)
{
	UE_LOG(LogLD33, Display, TEXT("%s (%s) rec %s"), *GetName(), *FString::FromInt(IsLeader), *FString::FromInt((int32)msg.Type));

	if (CurrentState != GamerState::GS_AttackingBoss && CurrentState != GamerState::GS_PlayerVersusPlayer && msg.Type == GamerMessageType::GMT_RequestMemberJoin && msg.Sender->Guild == this->Guild && msg.Sender->IsLeader && !IsLeader)
	{
		UE_LOG(LogLD33, Display, TEXT("%s is now following leader"), *GetName());
		CurrentState = GamerState::GS_FollowingLeader;

		if (FMath::RandRange(1, 3) == 1) SendGamerMessage(GamerMessageType::GMT_Ack);
	}

	if (CurrentState != GamerState::GS_AttackingBoss && CurrentState != GamerState::GS_ApproachingBoss && CurrentState != GamerState::GS_PlayerVersusPlayer && msg.Type == GamerMessageType::GMT_ReportBossUp && msg.Sender->Guild == this->Guild && IsLeader)
	{
		UE_LOG(LogLD33, Display, TEXT("%s is now looking for more"), *GetName());
		CurrentState = GamerState::GS_LookingForMore;

		if (FMath::RandRange(1, 2) == 1) SendGamerMessage(GamerMessageType::GMT_Ack);
	}

	if (msg.Type == GamerMessageType::GMT_AttackBossNow && CurrentState != GamerState::GS_PlayerVersusPlayer && CurrentState != GamerState::GS_AttackingBoss && msg.Sender->Guild == Guild && msg.Sender->IsLeader)
	{
		UE_LOG(LogLD33, Display, TEXT("%s is attacking boss"), *GetName());
		CurrentState = GamerState::GS_AttackingBoss;

		if (FMath::RandRange(1, 3) == 1) SendGamerMessage(GamerMessageType::GMT_Ack);
	}
}

void ABaseGamer::Attack(AActor* target)
{
	auto c = Cast<AAIController>(GetController());

	check(c);

	float rangeToTargetSquared = FVector::DistSquared(target->GetActorLocation(), GetActorLocation());

	if (MeleeAttack > MagicAttack)
	{
		if (rangeToTargetSquared > FMath::Square(200))
		{
			c->MoveToActor(target, 150);
		}
		else
		{
			target->TakeDamage(MeleeAttack * 2400, FDamageEvent(), c, this);
			IsCurrentlyAttacking = true;
		}
	}
	else 
	{
		if (rangeToTargetSquared > FMath::Square(2500))
		{
			c->MoveToActor(target, 2400);
		}
		else
		{
			FActorSpawnParameters p;
			p.Instigator = this;
			auto ib = GetWorld()->SpawnActor<AIceBolt>(IceBoltType, GetActorLocation(), FRotator::ZeroRotator);

			if (ib)
			{
				ib->Target = target;
				ib->Align();
				ib->DamageOnHit = MagicAttack * 3500;
			}

			IsCurrentlyAttacking = true;
		}
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

	FHitResult hit;

	GetWorld()->LineTraceSingleByChannel(hit, FVector(0, 0, 50000), FVector(0, 0, 0), ECollisionChannel::ECC_WorldStatic);

	SetActorLocation(FMath::RandPointInBox(FBox(FVector(-500, -500, hit.ImpactPoint.Z), FVector(500, 500, hit.ImpactPoint.Z))));
	CurrentState = GamerState::GS_IdleInTown;
}