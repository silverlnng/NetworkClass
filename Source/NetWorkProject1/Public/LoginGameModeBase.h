// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoginGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT1_API ALoginGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,Category="Mysettings");
	TSubclassOf<class UUserWidget> loginWidget;
};
