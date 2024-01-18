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
	//캐스팅해서 가져오기
	text_ammo->SetText(FText::AsNumber(0));
	btn_exitSession->OnClicked.AddDynamic(this,&UBattleWidget::OnexitSession);
	text_PlayerList->SetText(FText::FromString(FString(TEXT("")))); //공백으로 초기화 
}

void UBattleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(player!=nullptr)
	{
		int32 ammoCount = player->GetAmmo();
		text_ammo->SetText(FText::AsNumber(ammoCount));
		
		TArray<APlayerState*> players = GetWorld()->GetGameState<ANetGameStateBase>()->GetMyPlayerList();
		//월드에서 게임스테이트 가져와서 배열에 넣고
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
	//정방향 , 역방향 애니메이션 실행 가능
}

void UBattleWidget::ShowButtons()
{
	btn_exitSession->SetVisibility(ESlateVisibility::Visible);
	//SetVisibility 가 UWidget 에 선언 되어있음
	
}

void UBattleWidget::AddPlayerList(FString playerName,float score)
{
	playerList.Append(FString::Printf(TEXT("%s score: %d \n"),*playerName,(int32)score));
	// 이름 추가하고 한줄 띄루고 계속 추가 ...
	text_PlayerList->SetText(FText::FromString(FString(playerList)));
	
}

void UBattleWidget::OnexitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitSession();
}
