// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetworkGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT1_API ANetworkGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	//ChoosePlayerStart �� �ƴ϶� ChoosePlayerStart_Implementation �� override �ϴ� ����
private:
	int32 index = 0;
};
