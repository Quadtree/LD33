// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GamerMessage.generated.h"

enum class GamerMessageType
{
	GMT_ReportBossUp,
	GMT_RequestMemberJoin
};

class LD33_API FGamerMessage
{
	GENERATED_BODY()
public:
	FString ToString();

	class ABaseGamer* Sender;

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

class LD33_API FGMReportBossUp : public FGamerMessage
{
public:
	virtual FString ToString() override
	{
		return RT("lord flamenar is up", "flam is up", "i just noticed that LF is up");
	}
};

class LD33_API FGMRequestMemberJoin : public FGamerMessage
{
public:
	virtual FString ToString() override
	{
		return RT("lord flamenar is up", "flam is up", "LFM lord flam", "get in here!");
	}
};
