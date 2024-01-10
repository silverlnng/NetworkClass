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
	//세션에 관련된 모든 컨트롤을 하는 기본클래스
	void CreateSession(FString roomName,FString hostName, int32 playerCount);		// 요청한 결과를 받기 위한 함수
	void FindSession();

	//델리게이트 변수 선언
	FSearchSessionDele onCreateSlot;
	
	
private:
	FName mySessionName = FName("first Session");
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	void OnCreatedSession(FName sessionName, bool bWasSuccessful);	//서버에 세션생성을 요청하기 위한 함수
	void OnFoundSession(bool bwasSuccessful);
};
