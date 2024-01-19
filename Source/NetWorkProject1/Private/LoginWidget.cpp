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
	
	//��ư�� �Լ��� ���� (bind)
	btn_create->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedCreateButton);
	btn_back->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedBackButton);
	btn_MoveToCreatePanel->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedMoveToCreate);
	btn_MoveToFindPanel->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedMoveToFind);
	btn_Back2->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedBackButton);
	btn_findSessions->OnClicked.AddDynamic(this,&ULoginWidget::OnClickedFindSessionsButton);
	btn_Next->OnClicked.AddDynamic(this,&ULoginWidget::SetUserNameAndNext);

	
	gi = GetGameInstance<UNetworkGameInstance>();//���������ӿ�ũ�� �������� ����

	if(gi !=nullptr)
	{
		//����ã�� ��������Ʈ �Լ��� �����Ѵ�
		gi->onCreateSlot.AddDynamic(this,&ULoginWidget::OnSlotCreated);
		gi->onNewSearchComplete.AddDynamic(this,&ULoginWidget::OnClearScrollBox);
		gi->onFindButtonToggle.AddDynamic(this,&ULoginWidget::FindBtnToggle);
	}
	
	//�����̴� ���� ����ɶ����� ����� �Լ� ����=> ������ϸ� ����Ʈ���� 0�� ���� !
	sl_maxPlayers->OnValueChanged.AddDynamic(this,&ULoginWidget::OnSliderMoved);
	sl_maxPlayers->SetValue(2); //�θ��� ����Ʈ�� 
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
	// 0->2 ������
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

	//�ߺ�Ŭ�� (stuck �� �ɸ������� ) ������ ���� ��ư�� ��� ��Ȱ��ȭ  
	//btn_findSessions->SetIsEnabled(false);
}

void ULoginWidget::OnSlotCreated(FString roomName, FString hostName, int32 curPlayer, int32 maxPlayer, int32 pingSpeed,
	int32 sessionIdx)
{
	//�����κ��� ���� ������ ���� ������ ���� �߰��Ѵ�.
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
	//�ߺ��ǰų� ���ŵ� ���� ��Ͽ��� ������Ѵ�.
	//�������������� ��� ����� �����Ѵ�
	sb_roomList->ClearChildren();
}

void ULoginWidget::FindBtnToggle(bool value)
{
	btn_findSessions->SetIsEnabled(value);
}

void ULoginWidget::SetUserNameAndNext()
{
	gi->SetSessionName(editText_userName->GetText().ToString());
	//�����ν��Ͻ��� �Է����̸���(editText_userName) �����̸����� ����
	
	ws_widgetSwitcher->SetActiveWidgetIndex(1);
}

