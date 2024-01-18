// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT1_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UEditableText* eText_roomName;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UEditableText* eText_hostName;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class USlider* sl_maxPlayers;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UTextBlock* text_maxPlayers;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_create;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_back;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UWidgetSwitcher* ws_widgetSwitcher;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_MoveToCreatePanel;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_MoveToFindPanel;

	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UScrollBox* sb_roomList;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_Back2;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_findSessions;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UEditableText* editText_userName;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_Next;
	
	UPROPERTY(EditAnywhere,Category="MySettings")
	TSubclassOf<class USessionSlotWidget> slotWidget;

private:
	class UNetworkGameInstance* gi;
	//���������ӿ�ũ �ý����� �ٿ��� ������ ���־�!

	UFUNCTION()
	void OnClickedCreateButton();
	UFUNCTION()
	void OnClickedBackButton();
	UFUNCTION()
	void OnSliderMoved(float value);
	UFUNCTION()
	void OnClickedMoveToCreate();
	UFUNCTION()
	void OnClickedMoveToFind();
	UFUNCTION()
	void OnClickedFindSessionsButton();
	
	UFUNCTION()
	void OnSlotCreated(FString roomName,FString hostName,int32 curPlayer,
int32 maxPlayer,int32  pingSpeed,int32 sessionIdx);
	//�����ν��Ͻ��� �ִ� ��������Ʈ�� ������ �Լ�

	UFUNCTION()
	void OnClearScrollBox();
	UFUNCTION()
	void FindBtnToggle(bool value);

	UFUNCTION()
	void SetUserNameAndNext();
	
};