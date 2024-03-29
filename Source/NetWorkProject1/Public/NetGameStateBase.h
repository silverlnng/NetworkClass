// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT1_API ANetGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	TArray<class APlayerState*> GetMyPlayerList();

	//predicate에 넣을 조건식 ! static으로 선언해야한다 !
	static bool AscendingByString(const APlayerState& ps1,const APlayerState& ps2);
	static bool DescendingByString(const APlayerState& ps1,const APlayerState& ps2);
	
	static bool DescendingByScore(const APlayerState& ps1,const APlayerState& ps2);
	static bool AscendingByScore(const APlayerState& ps1,const APlayerState& ps2);
};
