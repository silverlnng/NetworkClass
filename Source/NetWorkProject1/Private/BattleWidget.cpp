// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"
#include "Components//TextBlock.h"
#include "Components/Button.h"
#include "NetWorkProject1/NetWorkProject1Character.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = GetOwningPlayerPawn<ANetWorkProject1Character>();
	//캐스팅해서 가져오기
	text_ammo->SetText(FText::AsNumber(0));
	btn_exitSession->OnClicked.AddDynamic(this,&UBattleWidget::OnexitSession);
}

void UBattleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(player!=nullptr)
	{
		int32 ammoCount = player->GetAmmo();
		text_ammo->SetText(FText::AsNumber(ammoCount));
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

void UBattleWidget::OnexitSession()
{
	
}
