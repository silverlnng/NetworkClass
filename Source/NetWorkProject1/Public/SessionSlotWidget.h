// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT1_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
		virtual void NativeConstruct() override;
	
public:
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="Mysettings")
	class UTextBlock* text_roomName;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="Mysettings")
	class UTextBlock* text_hostName;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="Mysettings")
	class UTextBlock* text_playerCounts;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="Mysettings")
	class UTextBlock* text_ping;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="Mysettings")
	class UButton* btn_join;

	UPROPERTY(VisibleAnywhere,Category="Mysettings")
	int32 roomIndex = 0;
	
	void SetSessionInfo(const FString roomName,const FString hostName,const int32 curPlayer, const int32 maxPlayer, 
	const int32 pingSpeed,const int32 idx);
	
private:
	class UNetworkGameInstance* gi;
	
	UFUNCTION()
	void OnClickedJoinButton();
	
};
