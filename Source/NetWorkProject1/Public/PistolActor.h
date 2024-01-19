// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PistolActor.generated.h"

UCLASS()
class NETWORKPROJECT1_API APistolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APistolActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,Category="MySettings")
	class UBoxComponent* boxComp;
	
	UPROPERTY(EditAnywhere,Category="MySettings")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere,Category="MySettings")
	TSubclassOf<class ABullectActor> bullect_bp;
	
	UPROPERTY(EditAnywhere,Category="MySettings")
	int32 ammo = 10;
	UPROPERTY(EditAnywhere,Category="MySettings")
	float damagePower = 2.f;
	UPROPERTY(EditAnywhere,Category="MySettings")
	float attackDelay = 1.f;
	
	void ReleaseWeapon(class ANetWorkProject1Character* player);
	void Fire(class ANetWorkProject1Character* player);
	
private:
	UFUNCTION()
	void OnOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void GrabPistol(class ANetWorkProject1Character* player);
	void SetCollisionResponse();
	
	UFUNCTION(Server,Unreliable)
	void ServerGrabPistol(class ANetWorkProject1Character* player);
	UFUNCTION(NetMulticast,Unreliable)
	void MulticastGrabPistol(class ANetWorkProject1Character* player);

	UFUNCTION(Server,Unreliable)
	void ServerReleasePistol(class ANetWorkProject1Character* player);
	UFUNCTION(NetMulticast,Unreliable)
	void MulticastReleasePistol(class ANetWorkProject1Character* player);
	
};
