// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"

#include "NetGameStateBase.h"
#include "NetworkGameInstance.h"
#include "NetworkPlayerController.h"
#include "Components//TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/GameStateBase.h"
#include "NetWorkProject1/NetWorkProject1Character.h"
#include "GameFramework/PlayerState.h"
#include "Components/HorizontalBox.h"


void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = GetOwningPlayerPawn<ANetWorkProject1Character>();
	//ĳ�����ؼ� ��������
	text_ammo->SetText(FText::AsNumber(0));
	btn_exitSession->OnClicked.AddDynamic(this,&UBattleWidget::OnexitSession);
	btn_Retry->OnClicked.AddDynamic(this,&UBattleWidget::OnRetry);
	
	text_PlayerList->SetText(FText::FromString(FString(TEXT("")))); //�������� �ʱ�ȭ

	currentTime=spectatorTime;
}

void UBattleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(player!=nullptr)
	{
		int32 ammoCount = player->GetAmmo();
		text_ammo->SetText(FText::AsNumber(ammoCount));
		
		TArray<APlayerState*> players = GetWorld()->GetGameState<ANetGameStateBase>()->GetMyPlayerList();
		//���忡�� ���ӽ�����Ʈ �����ͼ� �迭�� �ְ�
		playerList="";
		for(APlayerState* ps :players)
		{
			AddPlayerList(ps->GetPlayerName(),ps->GetScore());
		}
	}
	if(bProcessTimer)
	{
		if(currentTime>0)
		{
			currentTime-=InDeltaTime;
		}
		else
		{
			bProcessTimer=false;
			currentTime=spectatorTime;
			text_RespawnTimer->SetVisibility(ESlateVisibility::Hidden);
		}
		text_RespawnTimer->SetText(FText::AsNumber((int32)currentTime));
	}
}

void UBattleWidget::PlayHitAnimation()
{
	PlayAnimationForward(hitAnim);
	//������ , ������ �ִϸ��̼� ���� ����
}

void UBattleWidget::ShowButtons()
{
	//btn_exitSession->SetVisibility(ESlateVisibility::Visible);
	//SetVisibility �� UWidget �� ���� �Ǿ�����
	hb_menuButtons->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//visible �� �ϸ� ���� ������ �ִ� hb_menuButtons ���� ���ͷ����� ����
	//SelfHitTestInvisible ���� �ڽ��� ���ͷ����� ���� , �ڽ��� ���ͷ����� Ȱ��ȭ �ϱ� 
}

void UBattleWidget::AddPlayerList(FString playerName,float score)
{
	playerList.Append(FString::Printf(TEXT("%s score: %d \n"),*playerName,(int32)score));
	// �̸� �߰��ϰ� ���� ���� ��� �߰� ...
	text_PlayerList->SetText(FText::FromString(FString(playerList)));
	
}

void UBattleWidget::OnexitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitMySession();
}


void UBattleWidget::OnRetry()
{
	UE_LOG(LogTemp, Warning, TEXT("%s(%d) : %s"), *FString(__FUNCTION__), __LINE__,*FString("OnRetryClicked"));
	
	//��ư�� ������ ����� Ŭ���̾�Ʈ �ɼ��� �־ ChangeCharToSpectator �� rpc�Լ��� �����  
	ANetworkPlayerController* pc =player->GetController<ANetworkPlayerController>();
	pc->ChangeCharToSpectator();
	//�Էµ�
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly());
	hb_menuButtons->SetVisibility(ESlateVisibility::Hidden);

	text_RespawnTimer->SetVisibility(ESlateVisibility::Visible);
	bProcessTimer=true;

}