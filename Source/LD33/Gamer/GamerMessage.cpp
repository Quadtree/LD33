// Fill out your copyright notice in the Description page of Project Settings.

#include "LD33.h"
#include "GamerMessage.h"
#include "Guild.h"

FString FGamerMessage::ToString()
{
	switch (Type)
	{
	case GamerMessageType::GMT_ReportBossUp: return RT("lord flamenar is up", "flam is up", "i just noticed that LF is up");
	case GamerMessageType::GMT_RequestMemberJoin: return RT("lord flamenar is up", "flam is up", "LFM LF", "get in here!");
	case GamerMessageType::GMT_AttackBossNow: return RT("there he is!", "go now!", "pull!");
	case GamerMessageType::GMT_Ack: return RT("k", "kk", "got it", "ya");
	case GamerMessageType::GMT_Excuse: return RT("wtf lag", "sorry", "oops", "laaaaag", "wwwwwwwwwwwwwwddddd");
	case GamerMessageType::GMT_Punish: return RT("GET OUT OF THE CIRCLE!", "that's a 50 DKP MINUS!", "wtf dude", "dodget it, it's not hard!!");
	case GamerMessageType::GMT_Insult: 
		switch (FMath::RandRange(1, 4))
		{
		case 1: return "oh look its the " + TargetGuild->GuildName.ToLower();
		case 2: return "hi " + TargetGuild->GuildName.ToLower() + ", you suck";
		case 3: "hands off our kill " + TargetGuild->GuildName.ToLower();
		case 4: "get away ks brigade!";
		}
	}
	
	return "";
}