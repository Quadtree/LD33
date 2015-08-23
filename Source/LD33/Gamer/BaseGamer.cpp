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
#include "MeteorProjectile.h"
#include "GenericMob.h"


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
	
	RespawnTimer = 0;
	OriginalMeshTransform = GetMesh()->GetRelativeTransform();
}

void ABaseGamer::GamerInit()
{
	MaxHealth = BaseMaxHealth;
	Health = MaxHealth;
	Armor = BaseArmor;
	MeleeAttack = BaseMeleeAttack;
	MagicAttack = BaseMagicAttack;
	HealPower = BaseHealPower;

	TArray<FString> syl;

	FFileHelper::LoadANSITextFileToStrings(*(FPaths::GameContentDir() + "Data/NameSyllables.txt"), nullptr, syl);

	if (syl.Num() && GamerName.Len() == 0)
	{
		while (true)
		{
			FString newName = "";

			while (FMath::RandRange(1, 4) != 1)
			{
				newName += syl[FMath::Rand() % syl.Num()];
			}

			if (newName.Len() > 9 || newName.Len() < 4) continue;

			bool isMaleName = !newName.EndsWith("A") && !newName.EndsWith("E");

			if (isMaleName != IsMale) continue;

			GamerName = newName.Left(1) + newName.Right(newName.Len() - 1).ToLower();
			break;
		}

		UE_LOG(LogLD33, Display, TEXT("Name: %s"), *GamerName);
	}

	if (SkinLightness < 0.01f)
	{
		SkinLightness = FMath::FRandRange(0.25f, 1.f);

		HairColor.R = FMath::FRandRange(0.f, 1.f);
		HairColor.G = FMath::FRandRange(0.f, 1.f);
		HairColor.B = FMath::FRandRange(0.f, 1.f);
		HairColor.A = 1;
	}

	BlueprintInit();

	FTimerHandle h1, h2;

	//GetWorld()->GetTimerManager().SetTimer(h1, this, &ABaseGamer::UpdateMessageQueue, 0.5f, true, FMath::FRandRange(0, 0.5f));
	GetWorld()->GetTimerManager().SetTimer(h2, this, &ABaseGamer::UpdateState, 1.f, true, FMath::FRandRange(0, 1.f));
}


void ABaseGamer::UpdateMessageQueue()
{

}

void ABaseGamer::UpdateState()
{
	GetCharacterMovement()->SetAvoidanceEnabled(false);

	if (Health <= 0) return;

	IsCurrentlyAttacking = false;

	TArray<FOverlapResult> res;

	int32 numGuildMembersNearby = 0;

	float mostDamagedAllyHealth = FLT_MAX;
	AActor* mostDamagedAlly = nullptr;

	float distToNearestEnemey = FLT_MAX;
	AActor* nearestEnemy = nullptr;

	AGuild* detectedKSer = nullptr;

	if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(4000)))
	{
		for (auto a : res)
		{
			bool isEnemy = false;

			if (CurrentState == GamerState::GS_Farming && Cast<AGenericMob>(a.Actor.Get()) && Cast<AGenericMob>(a.Actor.Get())->Health > 0)
			{
				isEnemy = true;
			}

			if (CurrentState == GamerState::GS_PlayerVersusPlayer && Cast<ABaseGamer>(a.Actor.Get()) && Cast<ABaseGamer>(a.Actor.Get())->Guild != Guild && Cast<ABaseGamer>(a.Actor.Get())->Health > 0)
			{
				isEnemy = true;
			}

			if (isEnemy)
			{
				float dist = FVector::DistSquared(a.Actor->GetActorLocation(), GetActorLocation());
				
				if (dist < distToNearestEnemey)
				{
					distToNearestEnemey = dist;
					nearestEnemy = a.Actor.Get();
				}
			}

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

					if (HealPower >= 0.1f)
					{
						if (t->Health < (t->MaxHealth - 100) && t->Health > 0 && t->Health < mostDamagedAllyHealth)
						{
							mostDamagedAllyHealth = t->Health;
							mostDamagedAlly = t;
						}
					}
				}
				else
				{
					if (t->CurrentState == GamerState::GS_AttackingBoss && CurrentState == GamerState::GS_AttackingBoss)
					{
						detectedKSer = t->Guild;
						// hey they're KSing us!
						
					}
				}
			}
		}
	}

	if (detectedKSer && FMath::RandRange(1, 20))
	{
		SendGamerMessage(GamerMessageType::GMT_Insult, detectedKSer);
	}

	if (Health < (MaxHealth - 100) && HealPower >= 0.5f)
	{
		mostDamagedAlly = this;
	}

	if (mostDamagedAlly)
	{
		mostDamagedAlly->TakeDamage(-2000, FDamageEvent(), GetController(), this);
		OnCastHeal(mostDamagedAlly);
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
		//UE_LOG(LogLD33, Display, TEXT("%s LFM"), *GetName());

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
			nearestEnemy = *i;

			if (i->Health <= 0)
			{
				CurrentState = GamerState::GS_PlayerVersusPlayer;
			}
		}
	}

	if (CurrentState == GamerState::GS_Scouting && GetVelocity().SizeSquared() < FMath::Square(200))
	{
		if (auto c = Cast<AAIController>(GetController()))
		{
			auto t = c->MoveToLocation(Guild->GetPointOutsideOfTown(), 500);
		}
	}

	if (CurrentState == GamerState::GS_Farming && !nearestEnemy)
	{
		CurrentState = GamerState::GS_Scouting;
	}

	if (nearestEnemy)
	{
		
		if (MeleeAttack > HealPower || MagicAttack > HealPower)
		{
			Attack(nearestEnemy);
		}
		else
		{
			// if we get here it means i am a healer
			if (auto c = Cast<AAIController>(GetController()))
			{
				// try to stay within 30m
				auto t = c->MoveToLocation(nearestEnemy->GetActorLocation(), 3000);
			}

			GetCharacterMovement()->SetAvoidanceEnabled(true);
		}
	}

	for (TActorIterator<AMeteorProjectile> i(GetWorld()); i; ++i)
	{
		float distSquared = (i->GetActorLocation() - GetActorLocation()).SizeSquared2D();

		if (distSquared < FMath::Square(500))
		{
			UE_LOG(LogLD33, Display, TEXT("GET OUT THE WAY"));
			if (auto c = Cast<AAIController>(GetController()))
			{
				// GET OUT THE WAY
				auto t = c->MoveToLocation(FMath::RandPointInBox(FBox(FVector(-800,-800,0), FVector(800,800,0))) + GetActorLocation());
			}
		}
	}
}

void ABaseGamer::SendGamerMessage(GamerMessageType type, class AGuild* targetGuild)
{
	//UE_LOG(LogLD33, Display, TEXT("%s (%s) send %s"), *GetName(), *FString::FromInt(IsLeader), *FString::FromInt((int32)type));

	FGamerMessage msg;
	msg.Type = type;
	msg.Sender = this;
	msg.TargetGuild = targetGuild;

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

void ABaseGamer::SendInsult()
{
	if (LastInsultedMe)
		SendGamerMessage(GamerMessageType::GMT_Insult, LastInsultedMe);
}

void ABaseGamer::SendPunish()
{
	SendGamerMessage(GamerMessageType::GMT_Punish);
}

void ABaseGamer::ReceiveGamerMessage(const FGamerMessage& msg)
{
	//UE_LOG(LogLD33, Display, TEXT("%s (%s) rec %s"), *GetName(), *FString::FromInt(IsLeader), *FString::FromInt((int32)msg.Type));

	if (CurrentState != GamerState::GS_AttackingBoss && CurrentState != GamerState::GS_PlayerVersusPlayer && msg.Type == GamerMessageType::GMT_RequestMemberJoin && msg.Sender->Guild == this->Guild && msg.Sender->IsLeader && !IsLeader)
	{
		//UE_LOG(LogLD33, Display, TEXT("%s is now following leader"), *GetName());
		CurrentState = GamerState::GS_FollowingLeader;

		if (FMath::RandRange(1, 3) == 1) SendGamerMessage(GamerMessageType::GMT_Ack);
	}

	if (CurrentState != GamerState::GS_AttackingBoss && CurrentState != GamerState::GS_ApproachingBoss && CurrentState != GamerState::GS_PlayerVersusPlayer && msg.Type == GamerMessageType::GMT_ReportBossUp && msg.Sender->Guild == this->Guild && IsLeader)
	{
		//UE_LOG(LogLD33, Display, TEXT("%s is now looking for more"), *GetName());
		CurrentState = GamerState::GS_LookingForMore;

		if (FMath::RandRange(1, 2) == 1) SendGamerMessage(GamerMessageType::GMT_Ack);
	}

	if (msg.Type == GamerMessageType::GMT_AttackBossNow && CurrentState != GamerState::GS_PlayerVersusPlayer && CurrentState != GamerState::GS_AttackingBoss && msg.Sender->Guild == Guild && msg.Sender->IsLeader)
	{
		//UE_LOG(LogLD33, Display, TEXT("%s is attacking boss"), *GetName());
		CurrentState = GamerState::GS_AttackingBoss;

		if (FMath::RandRange(1, 3) == 1) SendGamerMessage(GamerMessageType::GMT_Ack);
	}

	if (msg.Type == GamerMessageType::GMT_Excuse && msg.Sender->Guild == Guild && IsLeader)
	{
		FTimerHandle h;
		GetWorld()->GetTimerManager().SetTimer(h, this, &ABaseGamer::SendPunish, FMath::FRandRange(0.35f, 1.2f));
	}

	if (msg.Type == GamerMessageType::GMT_Insult && Guild == msg.TargetGuild)
	{
		if (FMath::RandRange(1, 8) == 1)
		{
			// we are going to reply
			if (FMath::RandRange(1, 5) == 1 || FVector::DistSquared(GetActorLocation(), FVector(0,0,0)) < FMath::Square(1200))
			{
				FTimerHandle h;
				LastInsultedMe = msg.Sender->Guild;
				GetWorld()->GetTimerManager().SetTimer(h, this, &ABaseGamer::SendInsult, FMath::FRandRange(0.35f, 1.2f));
			}
			else
			{
				// reply WITH VIOLENCE
				CurrentState = GamerState::GS_PlayerVersusPlayer;
			}
		}
	}
}

void ABaseGamer::Attack(AActor* target)
{
	auto c = Cast<AAIController>(GetController());

	check(c);

	float rangeToTargetSquared = FVector::DistSquared(target->GetActorLocation(), GetActorLocation());

	if (MeleeAttack > MagicAttack)
	{
		if (rangeToTargetSquared > FMath::Square(400))
		{
			c->MoveToActor(target, 150);
		}
		else
		{
			target->TakeDamage(MeleeAttack * 4500, FDamageEvent(), c, this);
			OnMeleeAttack(target);
			IsCurrentlyAttacking = true;
		}
	}
	else 
	{
		GetCharacterMovement()->SetAvoidanceEnabled(true);

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
				ib->DamageOnHit = MagicAttack * 5000;
			}

			IsCurrentlyAttacking = true;
		}
	}
}

// Called every frame
void ABaseGamer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (Health <= 0)
	{
		RespawnTimer -= DeltaTime;

		if (RespawnTimer <= 0)
		{
			Respawn();
		}
	}
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
		if (Health > 0)
		{
			Damage = Damage * (1 - Armor);
			Health -= Damage;

			if (Health <= 0)
			{
				RespawnTimer = 10;
				GetMovementComponent()->SetActive(false);
				GetMesh()->SetSimulatePhysics(true);
				GetMesh()->SetPhysicsBlendWeight(1);

				for (TActorIterator<ALD33Character> i(GetWorld()); i; ++i)
				{
					i->Kills++;
				}
			}

			if (Cast<AMeteorProjectile>(DamageCauser) && FMath::RandBool())
			{
				SendGamerMessage(GamerMessageType::GMT_Excuse);
			}

			if (Cast<AGenericMob>(DamageCauser))
			{
				CurrentState = GamerState::GS_Farming;
			}

			if (Cast<ABaseGamer>(DamageCauser))
			{
				CurrentState = GamerState::GS_PlayerVersusPlayer;
			}
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
	//GetMovementComponent()->SetActive(true);

	//Health = MaxHealth;

	//FHitResult hit;

	//SetActorLocation(Guild->GetPointInTown());
	//CurrentState = GamerState::GS_Scouting;

	//GetMesh()->SetSimulatePhysics(false);
	//GetMesh()->SetSkeletalMesh(GetMesh()->GetSkeletalMesh());
	
	//GetMesh()->SetWorldRotation(GetRootComponent()->GetComponentRotation().Quaternion() + OriginalMeshTransform.GetRotation());
	//GetMesh()->SetWorldLocation(GetRootComponent()->GetComponentLocation() + OriginalMeshTransform.GetLocation());

	//GetMesh()->AttachTo(GetRootComponent());

	for (int i = 0; i < 100; ++i)
	{
		FVector np = Guild->GetPointInTown();

		ABaseGamer* rp = GetWorld()->SpawnActor<ABaseGamer>(this->GetClass(), np, FRotator::ZeroRotator);

		if (!rp) continue;

		rp->Guild = Guild;
		rp->GamerName = GamerName;
		rp->HairColor = HairColor;
		rp->SkinLightness = SkinLightness;
		rp->IsLeader = IsLeader;
		rp->IsMale = IsMale;
		rp->SpawnDefaultController();

		rp->GamerInit();

		check(rp->Health > 0);

		break;
	}

	Destroy();
}