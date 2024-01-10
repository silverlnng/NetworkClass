// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkGameInstance.generated.h"
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FSearchSessionDele,FString,roomName,FString,hostName,int32,curPlayer,
int32,maxPlayer,int32, pingSpeed,int32,sessionIdx);
UCLASS()
class NETWORKPROJECT1_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    virtual void Init() override;
	IOnlineSessionPtr sessionInterface;
	//���ǿ� ���õ� ��� ��Ʈ���� �ϴ� �⺻Ŭ����
	void CreateSession(FString roomName,FString hostName, int32 playerCount);		// ��û�� ����� �ޱ� ���� �Լ�
	void FindSession();

	//��������Ʈ ���� ����
	FSearchSessionDele onCreateSlot;
	
	
private:
	FName mySessionName = FName("first Session");
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	void OnCreatedSession(FName sessionName, bool bWasSuccessful);	//������ ���ǻ����� ��û�ϱ� ���� �Լ�
	void OnFoundSession(bool bwasSuccessful);
};
