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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FServerResponseDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindButtonStatus,bool, bisEnabled);
//_DYNAMIC : �������� ���̴� �� ����
UCLASS()
class NETWORKPROJECT1_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    virtual void Init() override;
	IOnlineSessionPtr sessionInterface;
	//���ǿ� ���õ� ��� ��Ʈ���� �ϴ� �⺻Ŭ����
	
	void CreateMySession(FString roomName,FString hostName, int32 playerCount);		// ��û�� ����� �ޱ� ���� �Լ�
	void FindMySession();
	void JoinMySession(int32 roomNumber);
	void ExitMySession();
	void SetSessionName(FString name);
	FORCEINLINE FString GetSessionName(){return mySessionName.ToString();}

	
	//��������Ʈ ���� ����
	//�������Ʈ���� ����ϰ� ������� ,
	UPROPERTY(EditAnywhere, BlueprintAssignable,Category="MySettings")
	FSearchSessionDele onCreateSlot;
	UPROPERTY(EditAnywhere, BlueprintAssignable,Category="MySettings")
	FServerResponseDele onNewSearchComplete;
	UPROPERTY(EditAnywhere, BlueprintAssignable,Category="MySettings")
	FFindButtonStatus onFindButtonToggle;
	
private:
	FName mySessionName = FName("first Session");
	TSharedPtr<FOnlineSessionSearch> SessionSearch;	//OnlineSessionSearch �� 
	
	void OnCreatedSession(FName sessionName, bool bWasSuccessful);	//������ ���ǻ����� ��û�ϱ� ���� �Լ�
	void OnFoundSession(bool bwasSuccessful);
	void OnJoinedSession(FName SesssionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroyedSesssion(FName sessionName, bool bwasSuccessful);
};
