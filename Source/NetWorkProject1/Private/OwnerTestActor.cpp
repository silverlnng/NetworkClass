// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnerTestActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AOwnerTestActor::AOwnerTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	rootComp =  CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootComp);

	meshComp =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	meshComp->SetupAttachment(rootComp);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AOwnerTestActor::BeginPlay()
{
	Super::BeginPlay();
	localRole = GetLocalRole();
	remoteRole = GetRemoteRole();
}

// Called every frame
void AOwnerTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintInfoLog();
}



void AOwnerTestActor::PrintInfoLog()
{
	FString localRoleString = *UEnum::GetValueAsString<ENetRole>(localRole);
	FString remoteRoleString = *UEnum::GetValueAsString<ENetRole>(remoteRole);
	FString ownerString = GetOwner() == nullptr ? *FString("No owner") : *GetOwner()->GetActorNameOrLabel();
	
	FString connectionString = GetNetConnection() ==nullptr? *FString("Invalid Connection") : *FString("Valid Connection");
		
	FString printString = FString::Printf(TEXT("LocalRole : %s \n remote Role : %s \n owner : %s \n connection: %s \n "),*localRoleString,*remoteRoleString,*ownerString,*connectionString);
	
	// 여기서 * 는 포인터가 아님 : 연산자 오버라이드 * 표시 => tchar형태로 반환된것
	// UEnum::GetValueAsString<ENetRole> : 값에 해당하는 enum값을 문자열로 
	DrawDebugString(GetWorld(),GetActorLocation(),printString,nullptr,FColor::Yellow,0,true,1.0f);
}
	
