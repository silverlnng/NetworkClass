// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"
#include "NetworkGameInstance.h"
#include "Net/UnrealNetwork.h"
void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UNetworkGameInstance* gi = GetGameInstance<UNetworkGameInstance>();
	
	//PlayerState ��pawn �̶� ���� ������
	//�����÷��̾ ������ �����ν��Ͻ��� ����� �����̸��� �����ͼ� ���� rpc���� �̸��������û .
	
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
	//playerState �� �����Ǿ��ִ� privateName ������ ���� ���� ������ ���� 
}
