// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "LD33HUD.h"
#include "CanvasItem.h"
#include "Classes/Engine/Canvas.h"
#include "EngineUtils.h"
#include "Gamer/BaseGamer.h"
#include "Guild.h"
#include "GenericMob.h"
#include "LD33Character.h"

void ALD33HUD::DrawHUD()
{
	Super::DrawHUD();

	//FCanvasTextItem txt(FVector2D(200,200), FText::FromString(TEXT("Test text")), NametagFont, FColor::Red);
	
	//Canvas->DrawItem(txt);

	//FFontRenderInfo inf;
	//inf.bEnabledShadow = true;

	FVector2D minimapSize = FVector2D(300, 300);
	FVector2D healthBarsPos = FVector2D(350, Canvas->ClipY - 110);
	FVector2D minimapPos = FVector2D(30, Canvas->ClipY - 30 - minimapSize.Y);
	FVector2D worldHalfSize = FVector2D(WORLD_SIZE, WORLD_SIZE);
	FVector2D overheadHealthBarSize = FVector2D(50, 4);

	const float HEALTH_BAR_BORDER_SIZE = 2;

	// draw nametags
	for (TActorIterator<AActor> i(GetWorld()); i; ++i)
	{
		FVector pos = Project(i->GetActorLocation());

		if (pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
		{
			float health = -1;
			float maxHealth = -1;

			if (auto gm = Cast<ABaseGamer>(*i))
			{
				{
					FCanvasTextItem txt(FVector2D(pos.X, pos.Y - 70), FText::FromString(gm->GamerName), NametagFont, FColor::Red);
					txt.bCentreX = true;
					Canvas->DrawItem(txt);
				}

				{
					FCanvasTextItem txt(FVector2D(pos.X, pos.Y - 50), FText::FromString("<" + (gm->Guild ? gm->Guild->GuildName : "???") + ">"), NametagFont, FColor::Red);
					txt.bCentreX = true;
					Canvas->DrawItem(txt);
				}
				health = gm->Health;
				maxHealth = gm->MaxHealth;
			}

			if (auto fi = Cast<AGenericMob>(*i))
			{
				health = fi->Health;
				maxHealth = fi->MaxHealth;
			}

			if (maxHealth > 0)
			{
				{
					FCanvasTileItem bx(FVector2D(pos.X, pos.Y - 20) - (overheadHealthBarSize / 2), overheadHealthBarSize, FColor::Black);
					Canvas->DrawItem(bx);
				}

				{
					FCanvasTileItem bx(FVector2D(pos.X, pos.Y - 20) - (overheadHealthBarSize / 2) + FVector2D(1, 1), (overheadHealthBarSize - FVector2D(2, 2)) * FVector2D(FMath::Max(health, 0.f) / maxHealth, 1), FColor::Red);
					Canvas->DrawItem(bx);
				}
			}
		}



		//
	}

	// draw chat messages
	for (int i = 0; i < ChatMessagePos.Num(); ++i)
	{
		if (GetWorld()->GetTimeSeconds() <= ChatMessageTimeLeft[i])
		{
			FVector pos = Project(ChatMessagePos[i]);

			if (pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
			{
				FCanvasTextItem txt(FVector2D(pos.X, pos.Y - 40), FText::FromString(ChatMessageText[i]), ChatMessageFont, FColor::White);
				txt.bCentreX = true;
				txt.EnableShadow(FColor::Black, FVector2D(1, 1));
				Canvas->DrawItem(txt);
			}
		}
		else
		{
			ChatMessagePos.RemoveAt(i);
			ChatMessageText.RemoveAt(i);
			ChatMessageTimeLeft.RemoveAt(i);
			--i;
		}
	}

	// draw the minimap
	FCanvasTileItem bx(minimapPos, minimapSize, FColor::Black);
	Canvas->DrawItem(bx);

	for (TActorIterator<ABaseGamer> i(GetWorld()); i; ++i)
	{
		FCanvasTileItem mic(minimapPos + FVector2D((i->GetActorLocation().Y + worldHalfSize.Y) / (worldHalfSize.Y * 2) * minimapSize.X, minimapSize.Y - (i->GetActorLocation().X + worldHalfSize.X) / (worldHalfSize.X * 2) * minimapSize.Y) - FVector2D(1, 1), FVector2D(3, 3), FColor::Red);

		Canvas->DrawItem(mic);
	}

	for (TActorIterator<AGenericMob> i(GetWorld()); i; ++i)
	{
		FCanvasTileItem mic(minimapPos + FVector2D((i->GetActorLocation().Y + worldHalfSize.Y) / (worldHalfSize.Y * 2) * minimapSize.X, minimapSize.Y - (i->GetActorLocation().X + worldHalfSize.X) / (worldHalfSize.X * 2) * minimapSize.Y) - FVector2D(1, 1), FVector2D(3, 3), FColor::Green);

		Canvas->DrawItem(mic);
	}

	ALD33Character* ch = nullptr;
	
	for (TActorIterator<ALD33Character> i(GetWorld()); i; ++i)
	{
		FCanvasTileItem mic(minimapPos + FVector2D((i->GetActorLocation().Y + worldHalfSize.Y) / (worldHalfSize.Y * 2) * minimapSize.X, minimapSize.Y - (i->GetActorLocation().X + worldHalfSize.X) / (worldHalfSize.X * 2) * minimapSize.Y) - FVector2D(4, 4), FVector2D(8, 8), FColor::White);
		Canvas->DrawItem(mic);
		ch = *i;
	}

	// draw the health bars
	if (ch)
	{
		{
			FCanvasTileItem bx(healthBarsPos, FVector2D(200, 35), FColor::Black);
			Canvas->DrawItem(bx);
		}

		{
			FCanvasTileItem bx(healthBarsPos + FVector2D(HEALTH_BAR_BORDER_SIZE, HEALTH_BAR_BORDER_SIZE), (FVector2D(200, 35) - FVector2D(HEALTH_BAR_BORDER_SIZE * 2, HEALTH_BAR_BORDER_SIZE * 2)) * FVector2D(FMath::Max(ch->Health, 0.f) / ch->MaxHealth, 1), FColor::Red);
			Canvas->DrawItem(bx);
		}

		{
			FCanvasTextItem txt(healthBarsPos + (FVector2D(200, 35) / 2), FText::FromString(FString::FromInt((int32)FMath::Max(ch->Health, 0.f)) + "/" + FString::FromInt((int32)ch->MaxHealth)), ChatMessageFont, FColor::White);
			txt.bCentreX = true;
			txt.bCentreY = true;
			txt.EnableShadow(FColor::Black, FVector2D(1, 1));
			Canvas->DrawItem(txt);
		}

		{
			FCanvasTileItem bx(healthBarsPos + FVector2D(0, 45), FVector2D(200, 35), FColor::Black);
			Canvas->DrawItem(bx);
		}

		{
			FCanvasTileItem bx(healthBarsPos + FVector2D(0, 45) + FVector2D(HEALTH_BAR_BORDER_SIZE, HEALTH_BAR_BORDER_SIZE), (FVector2D(200, 35) - FVector2D(HEALTH_BAR_BORDER_SIZE * 2, HEALTH_BAR_BORDER_SIZE * 2)) * FVector2D(FMath::Max(ch->Mana, 0.f) / ch->MaxMana, 1), FColor::Blue);
			Canvas->DrawItem(bx);
		}

		{
			FCanvasTextItem txt(healthBarsPos + FVector2D(0, 45) + (FVector2D(200, 35) / 2), FText::FromString(FString::FromInt((int32)FMath::Max(ch->Mana, 0.f)) + "/" + FString::FromInt((int32)ch->MaxMana)), ChatMessageFont, FColor::White);
			txt.bCentreX = true;
			txt.bCentreY = true;
			txt.EnableShadow(FColor::Black, FVector2D(1, 1));
			Canvas->DrawItem(txt);
		}
	}
}

void ALD33HUD::AddChatMessage(FVector pos, FString text)
{
	ChatMessagePos.Add(pos);
	ChatMessageText.Add(text);
	ChatMessageTimeLeft.Add(GetWorld()->GetTimeSeconds() + (text.Len() * 0.08f));
}
