// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleWidget.generated.h"

/**
 * 
 */

//플레이어들에게 정보를 가져와서 표시하는 위젯


UCLASS()
class NETWORKPROJECT1_API UBattleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual  void NativeConstruct() override;
	virtual  void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UTextBlock* text_ammo;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UTextBlock* text_PlayerList;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="MySettings")
	class UButton* btn_exitSession;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidgetAnim),Transient,Category="MySettings")	//Transient :휘발성 의미
	class UWidgetAnimation* hitAnim;
	//Transient :휘발성 의미 , 프레임 정보가 저장 안되길 바래서 설정

	void PlayHitAnimation();

	void ShowButtons();

	void AddPlayerList(FString playerName,float score);
	
private:
	
	class ANetWorkProject1Character* player;
	
	FString playerList;
	
	UFUNCTION()
	void OnexitSession();
};
