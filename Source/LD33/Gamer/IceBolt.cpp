// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "IceBolt.h"


// Sets default values
AIceBolt::AIceBolt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AIceBolt::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AIceBolt::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	Align();

	float moveDist = DeltaTime * 2000;

	if (FVector::DistSquared(Target->GetActorLocation(), GetActorLocation()) < FMath::Square(moveDist))
	{
		OnIceBoltHit();
		Target->TakeDamage(DamageOnHit, FDamageEvent(), GetInstigator() ? GetInstigator()->GetController() : nullptr, this);
		Destroy();
	}
	else
	{
		SetActorLocation(GetActorLocation() + GetActorRotation().RotateVector(FVector(moveDist, 0, 0)));
	}
}

void AIceBolt::Align()
{
	if (Target)
		SetActorRotation((Target->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation());
}