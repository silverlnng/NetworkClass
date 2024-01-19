// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMActor.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"


// Sets default values
ABGMActor::ABGMActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//TICK을 안쓰면 FALSE으로 설정
	rootComp=CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(rootComp);

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetupAttachment(RootComponent);
	audioComp->SetAutoActivate(true);
	//audioComp->set
}

// Called when the game starts or when spawned
void ABGMActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABGMActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

