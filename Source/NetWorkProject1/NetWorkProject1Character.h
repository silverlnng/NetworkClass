// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetWorkProject1Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetWorkProject1Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ReleaseWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Fire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_VoiceControl;
	
public:
	ANetWorkProject1Character();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	FORCEINLINE class APistolActor* GetOwningWeapon() {return owningWeapon;}
	
	FORCEINLINE APistolActor* SetOwningWeapon(class APistolActor* weapon) {return owningWeapon = weapon;}
	
	FORCEINLINE int32 GetAmmo(){return m_Ammo;}

	FORCEINLINE bool GetDeadState(){return bIsDead;}
	
	void setWeaponInfo(int32 ammo,float damage,float delay);

	void Damaged(int32 dmg);


	UPROPERTY(EditAnywhere,Category="MyComponents")
	class UWidgetComponent* playerInfoWidgetComp;
	
	
	UPROPERTY(EditAnywhere,Category="MySettings")
	TSubclassOf<class UBattleWidget> battlewidget;	//���� ������ ��������, ���� �Ҵ������


	UPROPERTY(EditAnywhere,Category="MySettings")
	TArray<UAnimMontage*> fireAnimMontage;

	UPROPERTY(EditAnywhere,Category="MySettings")
	int32 maxHealth = 10;

	UPROPERTY(EditAnywhere,Category="MySettings")
	TSubclassOf<class UCameraShakeBase> hitShake;
	
private:
	enum ENetRole localRole;
	enum ENetRole remoteRole;
	class UBattleWidget* battleUI;
	class UPlayerInfoWidget* info_UI;
	//TSubclassOf<class UBattleWidget>���� �����°� �����ͷ� ��� 
	//������ ���������� ������ �ν��Ͻ� ����Ű�� ���ؼ� ������ ���
	
	FTimerHandle fireCoolTime;
	bool bReloading = false;

	bool bIsDead=false;
	APlayerController* pc;
	
	UPROPERTY(Replicated)
	class APistolActor* owningWeapon;
	
	UPROPERTY(Replicated) // UPROPERTY(Replicated) : �� ������ ���� ������ ����� ������ ��
	float elapsedTime = 0; //����ð� 

	UPROPERTY(Replicated)
	int32 jumpCounts=0;

	UPROPERTY(Replicated)
	int32 m_Ammo = 0;
	UPROPERTY(Replicated)
	int32 m_damagePower = 0;
	UPROPERTY(Replicated)
	int32 m_attackDelay = 0;

	UPROPERTY(Replicated)
	int32 currentHealth = 0;

	// replicate�� ������ Ŭ���̾�Ʈ ��
	// ������������ ȣ��Ʈ �� �����϶� �� ���������� ����
	// ȣ��Ʈ�� ���� �����Ѱ� 
	// (������ ����  �޴µ� �� ����  ���Ҷ� ���� ) �Լ��� �����ϵ��� �ϱ�
	UPROPERTY(ReplicatedUsing=OnRep_JumpEffect)
	int32 repJumpCounts=0;
	
	void PrintInfoLog();
	void PrintTimeLog(float deltaSeconds);
	void JumpStart();
	
	void ReleaseWeapon();

	void Fire();

	void VoiceChatOn();
	void VoiceChatOff();

	UFUNCTION()
	void OnRep_JumpEffect(); //OnRep_ ���ξ� ���̱� 
	
	//RPC �Լ� 
	UFUNCTION(Server, Unreliable,WithValidation) //�ڵ��������� �������� �����ض� �� ������� . cpp������ _Implementation ���� ������ ���� 
	void ServerJump();
	
	UFUNCTION(NetMulticast,Unreliable)
	void MulticastJump();

	UFUNCTION(Server,Unreliable)
	void ServerFire();
	UFUNCTION(NetMulticast,Unreliable)
	void MulticastFire();
	//
	UFUNCTION(Server,Unreliable)
	void ServerDamaged(int32 dmg);
	UFUNCTION(Client,Unreliable)
	void ClientDamaged();
	//
	UFUNCTION(Server,Reliable)
	void ServerDieProcess();
	UFUNCTION(NetMulticast,Reliable)
	void NetMulticastDieProcess();
	
};	

