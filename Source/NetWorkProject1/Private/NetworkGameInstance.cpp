// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

void UNetworkGameInstance::Init()
{
	Super::Init();

	sessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if(sessionInterface != nullptr)
	{
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UNetworkGameInstance::OnCreatedSession);

		//oncreate , create
		//서버에 요청하고 응답을 받는데 지연시간이 있음
		//create하고 다만들어지면  create가 완료 되었을때 실행시킬 함수 OnCreatedSession 를 만들기 
	}

	//내 세션을 서버에 생성요청을 한다 ==> 서버를 만들어지고난뒤 세션요청을 해야함 => 먼저지연을 시켜야함
	FTimerHandle createHandler;

	GetWorld()->GetTimerManager().SetTimer(createHandler,FTimerDelegate::CreateLambda([&]()
	{
		CreateSession(mySessionName,5);
	}),2.0f,false);
	
}

void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp,Warning,TEXT("Session Name: %s") , *sessionName.ToString());
	UE_LOG(LogTemp,Warning,TEXT("Session create: %s") , bWasSuccessful ? *FString("Success!") : *FString("Fail"));
}

void UNetworkGameInstance::CreateSession(FName sessionName, int32 playerCount)
{
	FOnlineSessionSettings SessionSettings;	//SessionSettings구조체에 하나씩 설정값을 세팅
	
	SessionSettings.bIsDedicated = false; // 데디케이트 서버 사용안함
	SessionSettings.bAllowInvites = true; //초대기능 사용
	SessionSettings.bAllowJoinInProgress=true; // 진행중에도 들어오는것을 사용
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName()=="NULL"? true:false;
	// 접속하는 방법이 랜 경유 , 스팀서버 경유 두가지 있는데 랜 경유이면 null 문자열 반환, 스팀이면 steam 문자열 반환
	SessionSettings.bShouldAdvertise = true; //다른사람이 세션검색할경우 노출되도록 ( 검색이 가능하도록 )
	SessionSettings.bUseLobbiesIfAvailable=true;  //로비의 사용여부
	SessionSettings.NumPublicConnections=playerCount;
	//SessionSettings.NumPrivateConnections //호스트가 초대를 해야만 입장가능 
	
	sessionInterface->CreateSession(0,mySessionName,SessionSettings);
	//서버에  이런 세팅값으로 만들어달라는 요청 ( 호출시점 에 session이 완성된게 아님 )
	UE_LOG(LogTemp,Warning,TEXT("Try to create Session"));
}
