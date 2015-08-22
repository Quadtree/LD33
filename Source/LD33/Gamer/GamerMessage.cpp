// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "GamerMessage.h"

FString FGamerMessage::ToString()
{
	switch (Type)
	{
	case GamerMessageType::GMT_ReportBossUp: return RT("lord flamenar is up", "flam is up", "i just noticed that LF is up");
	case GamerMessageType::GMT_RequestMemberJoin: return RT("lord flamenar is up", "flam is up", "LFM LF", "get in here!");
	case GamerMessageType::GMT_AttackBossNow: return RT("there he is!", "go now!", "pull!");
	}
	
	return "";
}