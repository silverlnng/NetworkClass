// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetWorkProject1/NetWorkProject1Character.h"
#include "TestProject1Character.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT1_API ATestProject1Character : public ANetWorkProject1Character
{
	GENERATED_BODY()
public: 
	//virtual void setup
	ATestProject1Character();
	virtual void Tick(float DeltaSeconds) override;
	void InteractionPositive();
	void SensorUsingOvelap();
};





