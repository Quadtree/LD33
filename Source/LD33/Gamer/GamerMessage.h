// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

enum class GamerMessageType : uint8
{
	GMT_ReportBossUp,
	GMT_RequestMemberJoin,
	GMT_AttackBossNow,
	GMT_Ack,
	GMT_Max
};

struct LD33_API FGamerMessage
{
public:
	FString ToString();

	class ABaseGamer* Sender;

	GamerMessageType Type;

protected:
	FString RT(const char* a, const char* b)
	{
		switch (FMath::RandRange(1, 2))
		{
		case 1: return FString(a);
		case 2: return FString(b);
		}
		return FString("");
	}

	FString RT(const char* a, const char* b, const char* c)
	{
		switch (FMath::RandRange(1, 3))
		{
		case 1: return FString(a);
		case 2: return FString(b);
		case 3: return FString(b);
		}
		return FString("");
	}

	FString RT(const char* a, const char* b, const char* c, const char* d)
	{
		switch (FMath::RandRange(1, 2))
		{
		case 1: return FString(a);
		case 2: return FString(b);
		case 3: return FString(c);
		case 4: return FString(d);
		}
		return FString("");
	}

	FString RT(const char* a, const char* b, const char* c, const char* d, const char* e)
	{
		switch (FMath::RandRange(1, 2))
		{
		case 1: return FString(a);
		case 2: return FString(b);
		case 3: return FString(c);
		case 4: return FString(d);
		case 5: return FString(e);
		}
		return FString("");
	}
};
