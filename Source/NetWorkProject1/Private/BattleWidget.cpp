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
	//캐스팅해서 가져오기
	text_ammo->SetText(FText::AsNumber(0));
	btn_exitSession->OnClicked.AddDynamic(this,&UBattleWidget::OnexitSession);
	btn_Retry->OnClicked.AddDynamic(this,&UBattleWidget::OnRetry);
	
	text_PlayerList->SetText(FText::FromString(FString(TEXT("")))); //공백으로 초기화

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
		//월드에서 게임스테이트 가져와서 배열에 넣고
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
	//정방향 , 역방향 애니메이션 실행 가능
}

void UBattleWidget::ShowButtons()
{
	//btn_exitSession->SetVisibility(ESlateVisibility::Visible);
	//SetVisibility 가 UWidget 에 선언 되어있음
	hb_menuButtons->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//visible 을 하면 가장 상위에 있는 hb_menuButtons 에서 인터렉션을 막음
	//SelfHitTestInvisible 으로 자신의 인터렉션은 막고 , 자식의 인터렉션을 활성화 하기 
}

void UBattleWidget::AddPlayerList(FString playerName,float score)
{
	playerList.Append(FString::Printf(TEXT("%s score: %d \n"),*playerName,(int32)score));
	// 이름 추가하고 한줄 띄루고 계속 추가 ...
	text_PlayerList->SetText(FText::FromString(FString(playerList)));
	
}

void UBattleWidget::OnexitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitMySession();
}


void UBattleWidget::OnRetry()
{
	UE_LOG(LogTemp, Warning, TEXT("%s(%d) : %s"), *FString(__FUNCTION__), __LINE__,*FString("OnRetryClicked"));
	
	//버튼을 누르는 사람은 클라이언트 될수도 있어서 ChangeCharToSpectator 을 rpc함수로 만들기  
	ANetworkPlayerController* pc =player->GetController<ANetworkPlayerController>();
	pc->ChangeCharToSpectator();
	//입력도
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly());
	hb_menuButtons->SetVisibility(ESlateVisibility::Hidden);

	text_RespawnTimer->SetVisibility(ESlateVisibility::Visible);
	bProcessTimer=true;

}