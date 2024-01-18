// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleWidget.generated.h"

/**
 * 
 */

//�÷��̾�鿡�� ������ �����ͼ� ǥ���ϴ� ����


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
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidgetAnim),Transient,Category="MySettings")	//Transient :�ֹ߼� �ǹ�
	class UWidgetAnimation* hitAnim;
	//Transient :�ֹ߼� �ǹ� , ������ ������ ���� �ȵǱ� �ٷ��� ����

	void PlayHitAnimation();

	void ShowButtons();

	void AddPlayerList(FString playerName,float score);
	
private:
	
	class ANetWorkProject1Character* player;
	
	FString playerList;
	
	UFUNCTION()
	void OnexitSession();
};
