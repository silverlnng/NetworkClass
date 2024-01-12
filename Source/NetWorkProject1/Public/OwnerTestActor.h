// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OwnerTestActor.generated.h"

UCLASS()
class NETWORKPROJECT1_API AOwnerTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOwnerTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="MySettings")
	class USceneComponent* rootComp;
	
	UPROPERTY(EditAnywhere, Category="MySettings")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, Category="MySettings")
	float maxDistance = 600;
	
	/*UPROPERTY(EditAnywhere, Category="MySettings")
	TArray<float> lengthArray;*/
	
private:
	enum ENetRole localRole;
	enum ENetRole remoteRole;
	void PrintInfoLog();
	AActor* GetNearestDistance();
};
