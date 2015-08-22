// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "LD33HUD.h"
#include "CanvasItem.h"
#include "Classes/Engine/Canvas.h"
#include "EngineUtils.h"
#include "Gamer/BaseGamer.h"
#include "Guild.h"

void ALD33HUD::DrawHUD()
{
	Super::DrawHUD();

	//FCanvasTextItem txt(FVector2D(200,200), FText::FromString(TEXT("Test text")), NametagFont, FColor::Red);
	
	//Canvas->DrawItem(txt);

	//FFontRenderInfo inf;
	//inf.bEnabledShadow = true;

	for (TActorIterator<ABaseGamer> i(GetWorld()); i; ++i)
	{
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
}


