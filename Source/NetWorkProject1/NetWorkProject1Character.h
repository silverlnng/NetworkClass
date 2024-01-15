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
	FORCEINLINE AActor* GetOwningWeapon() {return owningWeapon;}
	FORCEINLINE AActor* SetOwningWeapon(AActor* weapon) {return owningWeapon = weapon;}
	
private:
	enum ENetRole localRole;
	enum ENetRole remoteRole;
	
	UPROPERTY(Replicated)
	AActor* owningWeapon;
	
	UPROPERTY(Replicated) // UPROPERTY(Replicated) : �� ������ ���� ������ ����� ������ ��
	float elapsedTime = 0; //����ð� 

	UPROPERTY(Replicated)
	int32 jumpCounts=0;
	
	void PrintInfoLog();
	void PrintTimeLog(float deltaSeconds);
	void JumpStart();
	
	void ReleaseWeapon(const FInputActionValue& Value);

	//RPC �Լ� 
	UFUNCTION(Server, Unreliable,WithValidation) //�ڵ��������� �������� �����ض� �� ������� . cpp������ _Implementation ���� ������ ���� 
	void ServerJump();
	
	UFUNCTION(NetMulticast,Unreliable)
	void MulticastJump();

};

