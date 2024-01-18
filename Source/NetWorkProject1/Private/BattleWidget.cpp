// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"

#include "NetGameStateBase.h"
#include "NetworkGameInstance.h"
#include "Components//TextBlock.h"
#include "Components/Button.h"
#include "GameFramework/GameStateBase.h"
#include "NetWorkProject1/NetWorkProject1Character.h"
#include "GameFramework/PlayerState.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = GetOwningPlayerPawn<ANetWorkProject1Character>();
	//ĳ�����ؼ� ��������
	text_ammo->SetText(FText::AsNumber(0));
	btn_exitSession->OnClicked.AddDynamic(this,&UBattleWidget::OnexitSession);
	text_PlayerList->SetText(FText::FromString(FString(TEXT("")))); //�������� �ʱ�ȭ 
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
}

void UBattleWidget::PlayHitAnimation()
{
	PlayAnimationForward(hitAnim);
	//������ , ������ �ִϸ��̼� ���� ����
}

void UBattleWidget::ShowButtons()
{
	btn_exitSession->SetVisibility(ESlateVisibility::Visible);
	//SetVisibility �� UWidget �� ���� �Ǿ�����
	
}

void UBattleWidget::AddPlayerList(FString playerName,float score)
{
	playerList.Append(FString::Printf(TEXT("%s score: %d \n"),*playerName,(int32)score));
	// �̸� �߰��ϰ� ���� ���� ��� �߰� ...
	text_PlayerList->SetText(FText::FromString(FString(playerList)));
	
}

void UBattleWidget::OnexitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitSession();
}
