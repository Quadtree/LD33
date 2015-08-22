// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "MeteorProjectile.h"
#include "Gamer/BaseGamer.h"


// Sets default values
AMeteorProjectile::AMeteorProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMeteorProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorHit.AddUniqueDynamic(this, &AMeteorProjectile::OnHit);
}

// Called every frame
void AMeteorProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMeteorProjectile::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("METEOR HIT"));

	TArray<FOverlapResult> res;

	if (GetWorld()->OverlapMultiByChannel(res, GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(300)))
	{
		for (auto a : res)
		{
			if (a.Actor.IsValid() && Cast<ABaseGamer>(a.Actor.Get()))
			{
				a.Actor->TakeDamage(5000, FDamageEvent(), GetInstigator() ? GetInstigator()->GetController() : nullptr, this);
			}
		}
	}

	Destroy();
}