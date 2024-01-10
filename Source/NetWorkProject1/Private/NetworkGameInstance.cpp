// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UNetworkGameInstance::Init()
{
	Super::Init();

	sessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	
	if(sessionInterface != nullptr)
	{
		//서버쪽에서 Delegate으로 이벤트 값을 받을 함수를 연결
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreatedSession);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UNetworkGameInstance::OnFoundSession);
		//oncreate , create
		//서버에 요청하고 응답을 받는데 지연시간이 있음
		//create하고 다만들어지면  create가 완료 되었을때 실행시킬 함수 OnCreatedSession 를 만들기 
	}

	//내 세션을 서버에 생성요청을 한다 ==> 서버를 만들어지고난뒤 세션요청을 해야함 => 먼저지연을 시켜야함
	/*FTimerHandle createHandler;

	GetWorld()->GetTimerManager().SetTimer(createHandler,FTimerDelegate::CreateLambda([&]()
	{
		CreateSession(mySessionName,5);
	}),2.0f,false);*/
	
}

void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp,Warning,TEXT("Session Name: %s") , *sessionName.ToString());
	UE_LOG(LogTemp,Warning,TEXT("Session create: %s") , bWasSuccessful ? *FString("Success!") : *FString("Fail"));
}

//(서버로 부터)세션 검색의 결과 이벤트 함수
void UNetworkGameInstance::OnFoundSession(bool bwasSuccessful)
{
	TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;	//Search의 결과를 TArray에 넣음
	
	UE_LOG(LogTemp,Warning,TEXT("Find Result : %s") , bwasSuccessful? *FString("success!"):*FString("Failed.."));
	
	if(bwasSuccessful)
	{
		int32 sessionNum = results.Num();
		UE_LOG(LogTemp,Warning,TEXT("Find Result : %d") ,results.Num());

		
		
		//for(const FOnlineSessionSearchResult& result : results)
		for(int32 i =0; i<results.Num();i++)	
		{
			FString foundRoomName;
			results[i].Session.SessionSettings.Get(FName("Room Name") , foundRoomName);
			FString foundHostName;
			results[i].Session.SessionSettings.Get(FName("Host Name") , foundHostName);

			// 세션의최대값 - 남아있는 자리 = 현재 플레이어수
			int32 maxPlayerCount = results[i].Session.SessionSettings.NumPublicConnections;
			int32 currentplayerCount = maxPlayerCount - results[i].Session.NumOpenPublicConnections;

			//데이터가 느린사람이 빠른속도에 맟추는건 불가능 .하지만 빠른속도의 사람이 느린사람 속도를 맞추는건 가능
			
			int pingSpeed = results[i].PingInMs;
			
			UE_LOG(LogTemp,Warning,TEXT("RoomNAme: %s \n Host Name: %s \n player count: (%d/%d) \n ping: %d ms \n\n"),*foundRoomName,*foundHostName,currentplayerCount,maxPlayerCount,pingSpeed);	//로그로 확인하기

			//델리게이트 이벤트 실행하기
			onCreateSlot.Broadcast(foundRoomName,foundHostName,currentplayerCount,maxPlayerCount,pingSpeed,i);
		}
	}
}

void UNetworkGameInstance::CreateSession(FString roomName, FString hostName, int32 playerCount)
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

	//커스텀 설정값을 추가하기
	SessionSettings.Set(FName("Room Name"),roomName,EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("Host Name"),hostName,EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
	
	sessionInterface->CreateSession(0,mySessionName,SessionSettings);
	//서버에  이런 세팅값으로 만들어달라는 요청 ( 호출시점 에 session이 완성된게 아님 )
	UE_LOG(LogTemp,Warning,TEXT("Try to create Session"));
	UE_LOG(LogTemp,Warning,TEXT("current platform : %s"),*IOnlineSubsystem::Get()->GetSubsystemName().ToString());
}

void UNetworkGameInstance::FindSession()
{
	// 세션 검색 조건을 설정 
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;	//쿼리가 LAN 일치를 위한 것인지 여부
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);
	// SEARCH_PRESENCE : 자기정보가 공개된 경우
	// SEARCH_미리만들어둔 매크로 이용하기 
	// OnlineSessionSearch ~ 이런부분은 스팀. 언리얼과 상관없는 부분은 원래의 c++ 사용 (스마트 포인터 사용)
	sessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}
