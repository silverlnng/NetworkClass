// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BullectActor.generated.h"

UCLASS()
class NETWORKPROJECT1_API ABullectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="MySetting")
	class USphereComponent* sphereComp;
	
	UPROPERTY(EditAnywhere, Category="MySetting")
	class UStaticMeshComponent* meshComp;
	
	UPROPERTY(EditAnywhere, Category="MySetting")
	float moveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category="MySetting")
	int32 damage = 0;
	
private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	
};
