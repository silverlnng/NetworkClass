// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"
#include "NetworkGameInstance.h"
#include "Net/UnrealNetwork.h"
void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UNetworkGameInstance* gi = GetGameInstance<UNetworkGameInstance>();
	
	//PlayerState 는pawn 이랑 같이 생성됨
	//로컬플레이어가 각자의 게임인스턴스에 저장된 세션이름을 가져와서 서버 rpc으로 이름등록을요청 .
	
	if (GetPlayerController() && GetPlayerController()->IsLocalPlayerController() && gi!=nullptr)
	{
		FString sessionName = gi->GetSessionName();
		UE_LOG(LogTemp,Warning,TEXT("sessionName :%s")  ,*sessionName);
		sesrverSetMyName(sessionName);
	}
	
}

void ANetPlayerState::sesrverSetMyName_Implementation(const FString& name)
{
	SetPlayerName(name);
	UE_LOG(LogTemp,Warning,TEXT("Set sessionName :%s")  ,*name);
	//playerState 에 구현되어있는 privateName 변수의 값을 들어온 값으로 갱신 
}
