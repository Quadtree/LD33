// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "LD33HUD.h"
#include "CanvasItem.h"
#include "Classes/Engine/Canvas.h"
#include "EngineUtils.h"
#include "Gamer/BaseGamer.h"
#include "Guild.h"
#include "LD33Character.h"

void ALD33HUD::DrawHUD()
{
	Super::DrawHUD();

	//FCanvasTextItem txt(FVector2D(200,200), FText::FromString(TEXT("Test text")), NametagFont, FColor::Red);
	
	//Canvas->DrawItem(txt);

	//FFontRenderInfo inf;
	//inf.bEnabledShadow = true;

	FVector2D minimapSize = FVector2D(300, 300);
	FVector2D minimapPos = FVector2D(30, Canvas->ClipY - 30 - minimapSize.Y);
	FVector2D worldHalfSize = FVector2D(25000, 25000);

	FCanvasTileItem bx(minimapPos, minimapSize, FColor::Black);

	Canvas->DrawItem(bx);

	for (TActorIterator<ABaseGamer> i(GetWorld()); i; ++i)
	{
		{
			FCanvasTileItem mic(minimapPos + FVector2D((i->GetActorLocation().Y + worldHalfSize.Y) / (worldHalfSize.Y * 2) * minimapSize.X, minimapSize.Y - (i->GetActorLocation().X + worldHalfSize.X) / (worldHalfSize.X * 2) * minimapSize.Y) - FVector2D(1, 1), FVector2D(3, 3), FColor::Red);

			Canvas->DrawItem(mic);
		}

		FVector pos = Project(i->GetActorLocation());

		if (pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
		{
			{
				FCanvasTextItem txt(FVector2D(pos.X, pos.Y - 60), FText::FromString(i->GamerName), NametagFont, FColor::Red);
				txt.bCentreX = true;
				Canvas->DrawItem(txt);
			}

			{
				FCanvasTextItem txt(FVector2D(pos.X, pos.Y - 40), FText::FromString("<" + (i->Guild ? i->Guild->GuildName : "???") + ">"), NametagFont, FColor::Red);
				txt.bCentreX = true;
				Canvas->DrawItem(txt);
			}
		}



		//
	}
	
	for (TActorIterator<ALD33Character> i(GetWorld()); i; ++i)
	{
		{
			FCanvasTileItem mic(minimapPos + FVector2D((i->GetActorLocation().Y + worldHalfSize.Y) / (worldHalfSize.Y * 2) * minimapSize.X, minimapSize.Y - (i->GetActorLocation().X + worldHalfSize.X) / (worldHalfSize.X * 2) * minimapSize.Y) - FVector2D(4, 4), FVector2D(8, 8), FColor::Green);
			Canvas->DrawItem(mic);
		}
	}

	for (int i = 0; i < ChatMessagePos.Num(); ++i)
	{
		if (GetWorld()->GetTimeSeconds() <= ChatMessageTimeLeft[i])
		{
			FVector pos = Project(ChatMessagePos[i]);

			if (pos.X >= -200 && pos.Y >= -200 && pos.X <= Canvas->ClipX + 200 && pos.Y <= Canvas->ClipY + 200)
			{
				FCanvasTextItem txt(FVector2D(pos.X, pos.Y - 40), FText::FromString(ChatMessageText[i]), ChatMessageFont, FColor::White);
				txt.bCentreX = true;
				txt.ShadowColor = FColor::Black;
				txt.ShadowOffset = FVector2D(1, -1);
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
}

void ALD33HUD::AddChatMessage(FVector pos, FString text)
{
	ChatMessagePos.Add(pos);
	ChatMessageText.Add(text);
	ChatMessageTimeLeft.Add(GetWorld()->GetTimeSeconds() + (text.Len() * 0.08f));
}
