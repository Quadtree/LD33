// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LD33.h"
#include "LD33PlayerController.h"
#include "LD33Character.h"
#include "Gamer/BaseGamer.h"
#include "AI/Navigation/NavigationSystem.h"

ALD33PlayerController::ALD33PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ALD33PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ALD33PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ALD33PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ALD33PlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("FrontalConeAttack", IE_Pressed, this, &ALD33PlayerController::FrontalConeAttack);
	InputComponent->BindAction("SoulDrainAttack", IE_Pressed, this, &ALD33PlayerController::SoulDrainAttack);
	InputComponent->BindAction("MeteorAttack", IE_Pressed, this, &ALD33PlayerController::MeteorAttack);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALD33PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALD33PlayerController::MoveToTouchLocation);
}

void ALD33PlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ALD33PlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ALD33PlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ALD33PlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ALD33PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ALD33PlayerController::FrontalConeAttack()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		auto chr = Cast<ALD33Character>(GetPawn());

		if (chr) chr->FrontalConeAttack(Hit.ImpactPoint);
	}
}

void ALD33PlayerController::SoulDrainAttack()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		TArray<FOverlapResult> res;

		if (GetWorld()->OverlapMultiByChannel(res, Hit.ImpactPoint, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(3000)))
		{
			float closestDistSquared = 10000000;
			ABaseGamer* closest = nullptr;

			for (auto a : res)
			{
				if (a.Actor.IsValid() && Cast<ABaseGamer>(a.Actor.Get()) && Cast<ABaseGamer>(a.Actor.Get())->Health > 0)
				{
					float distSquared = FVector::DistSquared(Hit.ImpactPoint, a.Actor->GetActorLocation());

					if (distSquared < closestDistSquared)
					{
						closestDistSquared = distSquared;
						closest = Cast<ABaseGamer>(a.Actor.Get());
					}
				}
			}

			if (closest)
			{
				auto chr = Cast<ALD33Character>(GetPawn());

				if (chr) chr->SoulDrainAttack(closest);
			}
		}
	}
}

void ALD33PlayerController::MeteorAttack()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		auto chr = Cast<ALD33Character>(GetPawn());

		if (chr) chr->MeteorAttack(Hit.ImpactPoint);
	}
}