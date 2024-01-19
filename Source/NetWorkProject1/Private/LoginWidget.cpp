// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "NetworkGameInstance.h"
#include  "Components/WidgetSwitcher.h"
#include  "SessionSlotWidget.h"
#include "Components/ScrollBox.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	sl_maxPlayers->SetMinValue(2.0f);
	sl_maxPlayers->SetMaxValue(10.0f);
	sl_maxPlayers->SetStepSize(1.0f);

	text_maxPlayers->SetText(FText::AsNumber(2));
	
	//버튼에 함수를 연결 (bind)
	btn_create->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedCreateButton);
	btn_back->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedBackButton);
	btn_MoveToCreatePanel->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedMoveToCreate);
	btn_MoveToFindPanel->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedMoveToFind);
	btn_Back2->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedBackButton);
	btn_findSessions->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedFindSessionsButton);
	btn_Next->OnClicked.AddDynamic(this,&ULoginWidget::SetUserNameAndNext);

	
	gi = GetGameInstance<UNetworkGameInstance>();//게임프레임워크는 가져오기 쉬움

	if(gi !=nullptr)
	{
		//세션찾기 델리게이트 함수를 연결한다
		gi->onCreateSlot.AddDynamic(this,&ULoginWidget::OnSlotCreated);
		gi->onNewSearchComplete.AddDynamic(this,&ULoginWidget::OnClearScrollBox);
		gi->onFindButtonToggle.AddDynamic(this,&ULoginWidget::FindBtnToggle);
	}
	
	//슬라이더 값이 변경될때마다 실행될 함수 연결=> 변경안하면 디폴트값이 0이 들어갔음 !
	sl_maxPlayers->OnValueChanged.AddDynamic(this,&ULoginWidget::OnSliderMoved);
	sl_maxPlayers->SetValue(2); //두명을 디폴트로 
}

void ULoginWidget::OnClickedCreateButton()
{
	if(gi!=nullptr)
	{
		gi->CreateMySession(eText_roomName->GetText().ToString(), eText_hostName->GetText().ToString(), (int32)sl_maxPlayers->GetValue());
	}
}

void ULoginWidget::OnClickedBackButton()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::OnSliderMoved(float value)
{
	text_maxPlayers->SetText(FText::AsNumber((int32)value));
}

void ULoginWidget::OnClickedMoveToCreate()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(2);
}

void ULoginWidget::OnClickedMoveToFind()
{
	// 0->2 번으로
	ws_widgetSwitcher->SetActiveWidgetIndex(3);
	if(gi!=nullptr)
	{
		gi->FindMySession();
	}
}

void ULoginWidget::OnClickedFindSessionsButton()
{
	if(gi!=nullptr)
	{
		gi->FindMySession();
	}

	//중복클릭 (stuck 에 걸릴수있음 ) 방지를 위해 버튼을 잠시 비활성화  
	//btn_findSessions->SetIsEnabled(false);
}

void ULoginWidget::OnSlotCreated(FString roomName, FString hostName, int32 curPlayer, int32 maxPlayer, int32 pingSpeed,
	int32 sessionIdx)
{
	//서버로부터 받은 정보로 슬롯 위젯을 만들어서 추가한다.
	if(slotWidget != nullptr)
	{
		if(USessionSlotWidget* slot_UI = CreateWidget<USessionSlotWidget>(GetWorld(), slotWidget))
		{
			slot_UI->SetSessionInfo(roomName,hostName,curPlayer,maxPlayer,pingSpeed,sessionIdx);
			sb_roomList->AddChild(slot_UI);
		}
	}
}

void ULoginWidget::OnClearScrollBox()
{
	//중복되거나 제거된 룸은 목록에서 없어야한다.
	//기존슬롯위젯을 모두 지우고 시작한다
	sb_roomList->ClearChildren();
}

void ULoginWidget::FindBtnToggle(bool value)
{
	btn_findSessions->SetIsEnabled(value);
}

void ULoginWidget::SetUserNameAndNext()
{
	gi->SetSessionName(editText_userName->GetText().ToString());
	//게임인스턴스에 입력한이름을(editText_userName) 세션이름으로 저장
	
	ws_widgetSwitcher->SetActiveWidgetIndex(1);
}

