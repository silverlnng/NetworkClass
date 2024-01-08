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
		//������ ��û�ϰ� ������ �޴µ� �����ð��� ����
		//create�ϰ� �ٸ��������  create�� �Ϸ� �Ǿ����� �����ų �Լ� OnCreatedSession �� ����� 
	}

	//�� ������ ������ ������û�� �Ѵ� ==> ������ ����������� ���ǿ�û�� �ؾ��� => ���������� ���Ѿ���
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
	
	sessionInterface->CreateSession(0,mySessionName,SessionSettings);
	//������  �̷� ���ð����� �����޶�� ��û ( ȣ����� �� session�� �ϼ��Ȱ� �ƴ� )
	UE_LOG(LogTemp,Warning,TEXT("Try to create Session"));
}
