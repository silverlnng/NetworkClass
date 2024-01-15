// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"
#include "Components//TextBlock.h"
#include "NetWorkProject1/NetWorkProject1Character.h"

void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	player = GetOwningPlayerPawn<ANetWorkProject1Character>();	//캐스팅해서 가져오기
	text_ammo->SetText(FText::AsNumber(0));
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
