// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

void UNetworkGameInstance::Init()
{
	Super::Init();

	sessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	
	if(sessionInterface != nullptr)
	{
		//�����ʿ��� Delegate���� �̺�Ʈ ���� ���� �Լ��� ����
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreatedSession);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UNetworkGameInstance::OnFoundSession);
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UNetworkGameInstance::OnJoinedSession);
		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this,&UNetworkGameInstance::OnDestroyedSesssion);
		//oncreate , create
		//������ ��û�ϰ� ������ �޴µ� �����ð��� ����
		//create�ϰ� �ٸ��������  create�� �Ϸ� �Ǿ����� �����ų �Լ� OnCreatedSession �� �����
		//OnJoinSessionCompleteDelegates : �Ű����� �ʿ� F12 ������ Ȯ���ϱ�  
	}

	//�� ������ ������ ������û�� �Ѵ� ==> ������ ����������� ���ǿ�û�� �ؾ��� => ���������� ���Ѿ���
	/*FTimerHandle createHandler;

	GetWorld()->GetTimerManager().SetTimer(createHandler,FTimerDelegate::CreateLambda([&]()
	{
		CreateSession(mySessionName,5);
	}),2.0f,false);*/
	
}


void UNetworkGameInstance::CreateSession(FString roomName, FString hostName, int32 playerCount)
{
	FOnlineSessionSettings SessionSettings;	//SessionSettings����ü�� �ϳ��� �������� ����
	
	SessionSettings.bIsDedicated = false; // ��������Ʈ ���� ������
	SessionSettings.bAllowInvites = true; //�ʴ��� ���
	SessionSettings.bAllowJoinInProgress=true; // �����߿��� �����°��� ���
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName()=="NULL"? true:false;
	// �����ϴ� ����� �� ���� , �������� ���� �ΰ��� �ִµ� �� �����̸� null ���ڿ� ��ȯ, �����̸� steam ���ڿ� ��ȯ
	
	SessionSettings.bShouldAdvertise = true; //�ٸ������ ���ǰ˻��Ұ�� ����ǵ��� ( �˻��� �����ϵ��� )
	SessionSettings.bUseLobbiesIfAvailable=true;  //�κ��� ��뿩��
	SessionSettings.NumPublicConnections=playerCount;
	//SessionSettings.NumPrivateConnections //ȣ��Ʈ�� �ʴ븦 �ؾ߸� ���尡��

	//Ŀ���� �������� �߰��ϱ�
	SessionSettings.Set(FName("Room Name"),roomName,EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("Host Name"),hostName,EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
	
	sessionInterface->CreateSession(0,mySessionName,SessionSettings);
	//������  �̷� ���ð����� �����޶�� ��û ( ȣ����� �� session�� �ϼ��Ȱ� �ƴ� )
	UE_LOG(LogTemp,Warning,TEXT("Try to create Session"));
	UE_LOG(LogTemp,Warning,TEXT("current platform : %s"),*IOnlineSubsystem::Get()->GetSubsystemName().ToString());
}

void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp,Warning,TEXT("Session Name: %s") , *sessionName.ToString());
	UE_LOG(LogTemp,Warning,TEXT("Session create: %s") , bWasSuccessful ? *FString("Success!") : *FString("Fail"));

	//��Ƽ�÷��̸� �� ������ �̵��Ѵ�. ���� ��� �ۼ����ֱ�
	///Script/Engine.World'/Game/Maps/BattleMap.BattleMap' ���� ��� ��� �� �־��ָ� ��
	GetWorld()->ServerTravel("/Game/Maps/BattleMap?Listen",true);
	//�������� ���������̱⶧����  ?listen ���� ���� 
}

void UNetworkGameInstance::FindSession()
{
	onFindButtonToggle.Broadcast(false);
	// ���� �˻� ������ ���� 
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;	//������ LAN ��ġ�� ���� ������ ����
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);
	// SEARCH_PRESENCE : �ڱ������� ������ ���
	// SEARCH_�̸������� ��ũ�� �̿��ϱ� 
	// OnlineSessionSearch ~ �̷��κ��� ����. �𸮾�� ������� �κ��� ������ c++ ��� (����Ʈ ������ ���)
	sessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}
//(������ ����)���� �˻��� ��� �̺�Ʈ �Լ�
void UNetworkGameInstance::OnFoundSession(bool bwasSuccessful)
{
	TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;	//Search�� ����� TArray�� ����
	
	UE_LOG(LogTemp,Warning,TEXT("Find Result : %s") , bwasSuccessful? *FString("success!"):*FString("Failed.."));
	
	if(bwasSuccessful)
	{
		int32 sessionNum = results.Num();
		UE_LOG(LogTemp,Warning,TEXT("Find Result : %d") ,results.Num());

		onNewSearchComplete.Broadcast();
		//Broadcast ����� ��� �Լ����� �˸��°� 
		
		//for(const FOnlineSessionSearchResult& result : results)
		for(int32 i =0; i<results.Num();i++)	
		{
			FString foundRoomName;
			results[i].Session.SessionSettings.Get(FName("Room Name") , foundRoomName);
			FString foundHostName;
			results[i].Session.SessionSettings.Get(FName("Host Name") , foundHostName);

			// �������ִ밪 - �����ִ� �ڸ� = ���� �÷��̾��
			int32 maxPlayerCount = results[i].Session.SessionSettings.NumPublicConnections;
			int32 currentplayerCount = maxPlayerCount - results[i].Session.NumOpenPublicConnections;

			//�����Ͱ� ��������� �����ӵ��� ���ߴ°� �Ұ��� .������ �����ӵ��� ����� ������� �ӵ��� ���ߴ°� ����
			
			int pingSpeed = results[i].PingInMs;
			
			UE_LOG(LogTemp,Warning,TEXT("RoomNAme: %s \n Host Name: %s \n player count: (%d/%d) \n ping: %d ms \n\n"),*foundRoomName,*foundHostName,currentplayerCount,maxPlayerCount,pingSpeed);	//�α׷� Ȯ���ϱ�

			//��������Ʈ �̺�Ʈ �����ϱ�
			onCreateSlot.Broadcast(foundRoomName,foundHostName,currentplayerCount,maxPlayerCount,pingSpeed,i);
		}

		onFindButtonToggle.Broadcast(true);
	}
}

void UNetworkGameInstance::JoinSession(int32 roomNumber)
{
	sessionInterface->JoinSession(0,mySessionName,SessionSearch->SearchResults[roomNumber]);
	//
}

void UNetworkGameInstance::ExitSession()
{
	sessionInterface->DestroySession(mySessionName);
	// JoinSession ��ü�� ���� ���¸� (�����͸� �������ֵ��� ) �ٲ۰� !
	//DestroySession ���¸� �����ϴ°�
}

void UNetworkGameInstance::SetSessionName(FString name)
{
	mySessionName = FName(*name);
}

void UNetworkGameInstance::OnJoinedSession(FName SesssionName, EOnJoinSessionCompleteResult::Type result)
{
	UE_LOG(LogTemp,Warning,TEXT("Joined Session: %s"),*SesssionName.ToString());
	switch(result)
	{
	case EOnJoinSessionCompleteResult::Success:
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Success"));
			
			APlayerController* pc = GetWorld()->GetFirstPlayerController();
			
			FString url;	//����� ������ ��� ���� FString ���� ����
			
			sessionInterface->GetResolvedConnectString(SesssionName,url,NAME_GamePort);
			// url �� ����� ���������� ����
			// NAME_GamePort :  �����⿡�� ��Ʈ�������� (�� ������ �ɼ����� ) �����ҷ��� ��Ʈ�ּ�(���� �ּ�)�� �ʿ�
			// GamePort�� �ǽð� ������ ���� ��Ʈ.  NAME_GamePort f12������ 289���� Ȯ��

			UE_LOG(LogTemp, Warning, TEXT("url : %s"), *url);
			
			pc->ClientTravel(url,TRAVEL_Absolute);
			// ClientTravel ( �� �Ǵ� ip �ּ� ���� , travel Ÿ�� )  
			// ���� url�� ip�ּҷ� �ε� �ʱ��� ������ ������...??
			//�� �������δ� ���� ! 
		}
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		UE_LOG(LogTemp, Warning, TEXT("SessionIsFull"));
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		UE_LOG(LogTemp, Warning, TEXT("SessionDoesNotExist"));
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		UE_LOG(LogTemp, Warning, TEXT("CouldNotRetrieveAddress"));
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		UE_LOG(LogTemp, Warning, TEXT("AlreadyInSession"));
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		UE_LOG(LogTemp, Warning, TEXT("UnknownError"));
		break;
	default:
		break;
	}
}

void UNetworkGameInstance::OnDestroyedSesssion(FName sessionName, bool bwasSuccessful)
{
	UE_LOG(LogTemp,Warning,TEXT("Destroy Session Name: %s") , bwasSuccessful ? *sessionName.ToString():*FString("failed..."));
	
	if(bwasSuccessful)
	{
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if(pc!=nullptr)
		{
			pc->ClientTravel(FString("/Game/Maps/LobbyMap"),ETravelType::TRAVEL_Absolute);
		}
	}
}

