// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"

#include "NetworkGameInstance.h"
#include "Components//TextBlock.h"
#include "Components/Button.h"
#include "NetWorkProject1/NetWorkProject1Character.h"


void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = GetOwningPlayerPawn<ANetWorkProject1Character>();
	//ĳ�����ؼ� ��������
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
	//������ , ������ �ִϸ��̼� ���� ����
}

void UBattleWidget::ShowButtons()
{
	btn_exitSession->SetVisibility(ESlateVisibility::Visible);
	//SetVisibility �� UWidget �� ���� �Ǿ�����
	
}

void UBattleWidget::OnexitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitSession();
}