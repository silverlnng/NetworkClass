// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetWorkProject1Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "net/UnrealNetwork.h" //��Ʈ��ũ���� ���
#include "PistolActor.h"
#include "BattleWidget.h"
#include "PlayerInfoWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetWorkProject1Character

ANetWorkProject1Character::ANetWorkProject1Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	PrimaryActorTick.bCanEverTick = true; //ƽ��� ���� ���� ! �̰� ������ tick�� ���� �������

	bReplicates = true;
	SetReplicateMovement(true);

	playerInfoWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Info Widget"));
	playerInfoWidgetComp->SetupAttachment(GetMesh());
	playerInfoWidgetComp->SetDrawSize(FVector2d(200.f));
}

void ANetWorkProject1Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	pc = GetController<APlayerController>();
	
	//Add Input Mapping Context
	if (pc)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	localRole = GetLocalRole();
	remoteRole = GetRemoteRole();
	info_UI =Cast<UPlayerInfoWidget>(playerInfoWidgetComp->GetWidget());
	//GetController()!=nullptr&&GetController()->IsLocalController() ���� �����÷��̾�  �˻� 
	if (battlewidget != nullptr && GetController() != nullptr && GetController()->IsLocalController())
	{
		battleUI = CreateWidget<UBattleWidget>(GetWorld(), battlewidget);
		if (battleUI != nullptr)
		{
			battleUI->AddToViewport(); //UI �� �ڽ��� UI �� ���� 
		}
	}

	if(HasAuthority())
	{
		currentHealth = maxHealth;
	}
	
}

void ANetWorkProject1Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PrintInfoLog();
	//PrintTimeLog(DeltaSeconds);


	if (info_UI != nullptr)
	{
		info_UI->pb_Health->SetPercent((float)currentHealth / (float)maxHealth);
	}

	if(!bIsDead && currentHealth <= 0 ) //ĳ������ ���ó�� tick �̾ bool ���Ǿ��� ������ �߻��Ҽ����� 
	{
		bIsDead=true;
		//�״� �ִϸ��̼� ���� -> AnimInstance ������ -> �ִϸ��̼� bp���� ������ ���� ���� 
			
		ServerDieProcess();
	}
}

void ANetWorkProject1Character::setWeaponInfo(int32 ammo, float damage, float delay)
{
	m_Ammo = ammo;
	m_damagePower = damage;
	m_attackDelay = delay;
}

#pragma region DamageRPC

void ANetWorkProject1Character::Damaged(int32 dmg)
{
	ServerDamaged(dmg);
	//���⼭�� Damaged() �� ���������� �߻��ؼ� ServerDamaged_Implementation ���ε� ����������
	//Ȯ�强�� ���ؼ�(��쿡 ���� ���ÿ����� Damaged() �߻��� ���ɼ��� �������� ���� ) ServerDamaged ->ServerDamaged_Implementation ������ ��ġ��
	// ServerDamaged �� ���� , ���� ��� ���డ��
	//ServerDamaged_Implementation�� ���������� ���డ���� 
}

void ANetWorkProject1Character::ServerDamaged_Implementation(int32 dmg)
{
	currentHealth = FMath::Max(0, currentHealth - dmg);
	// currentHealth �� replicated �Ǵ� �����̰� , ��ο��� �����ϰ� �������Ѵ� => �������� ����
	//FMath::Max �� ���߿� ��ū���� ��ȯ�ϴ� �Լ�
	
	ClientDamaged();
	//damaged ��  �����ϴ� Ŭ���̾�Ʈ "��" UI�� �����Ű���� �� 
}

void ANetWorkProject1Character::ClientDamaged_Implementation()
{
	battleUI->PlayHitAnimation();
	
	//ī�޶� ����ũ ȿ�� �� �ֱ�
	//APlayerController* pc = GetController<APlayerController>();
	if(pc!=nullptr && hitShake!= nullptr)
	{
		pc->ClientStartCameraShake(hitShake);
	}
}

#pragma endregion 

void ANetWorkProject1Character::PrintInfoLog()
{
	/*
	//��Ʈ���� role Ȯ�ο� �α�
	FString localRoleString = *UEnum::GetValueAsString<ENetRole>(localRole);
	FString remoteRoleString = *UEnum::GetValueAsString<ENetRole>(remoteRole);
	FString ownerString = GetOwner() == nullptr ? *FString("No owner") : *GetOwner()->GetActorNameOrLabel();

	FString connectionString = GetNetConnection() == nullptr
		                           ? *FString("Invalid Connection")
		                           : *FString("Valid Connection");

	FString printString = FString::Printf(
		TEXT("LocalRole : %s \n remote Role : %s \n owner : %s \n connection: %s \n "), *localRoleString,
		*remoteRoleString, *ownerString, *connectionString);

	// ���⼭ * �� �����Ͱ� �ƴ� : ������ �������̵� * ǥ�� => tchar���·� ��ȯ�Ȱ�
	// UEnum::GetValueAsString<ENetRole> : ���� �ش��ϴ� enum���� ���ڿ��� 
	DrawDebugString(GetWorld(), GetActorLocation(), printString, nullptr, FColor::White, 0, true, 1.0f);
	*/

	//���������� ��ũ Ȯ�ο� �α�
	FString gameModeString = GetWorld()->GetAuthGameMode()!=nullptr ? *FString("validGameMode"):*FString("invalidGameMode"); 
	//GetAuthGameMode : ���� ���忡 �ش��ϴ� ���Ӹ�� 
	//���帶�� ���Ӹ�� �����ϴϱ�(override ) , ���°�� ����Ʈ �� GameMode ���

	FString gameStateString = GetWorld()->GetGameState()!=nullptr ? *FString("validGameState"):*FString("invalidGameState");

	FString playerStateString = GetPlayerState()!=nullptr ? *FString("validPlayerState"):*FString
	("invalidPlayerState");

	AHUD* hud = GetController<APlayerController>()!=nullptr ? GetController<APlayerController>()->GetHUD():nullptr;
	//AHUD* hud =GetController<APlayerController>() != nullptr ? GetController<APlayerController>()->GetHUD() : nullptr;
	FString HUDString = hud !=nullptr ? *FString("validHUD"):*FString("invalidHUD");

	FString printString = FString::Printf(TEXT("gameMode : %s \n gameState : %s \n playerState : %s \n HUDS: %s \n"), *gameModeString,*gameStateString, *playerStateString, *HUDString);
	DrawDebugString(GetWorld(),GetActorLocation(),printString,nullptr,FColor::Black,0,true,1.0f);
}

void ANetWorkProject1Character::PrintTimeLog(float deltaSeconds)
{
	//GetLocalRole() == ROLE_Authority ���������� �����ϸ�ȴ�
	//�������� GetLocalRole �� ��� ROLE_Authority
	// ���ÿ��� GetLocalRole() == ROLE_Authority ���� 
	if (GetLocalRole() == ROLE_Authority)
	{
		elapsedTime += deltaSeconds;
	}

	//�����̾����� Ŭ���̾�Ʈ�� lapsedTime �� �������� ������ elapsedTime���� ����� �Ǵ� �� 
	// 
	//UE_LOG(LogTemp,Warning,TEXT("Elapsed Time :%.2f"),elapsedTime);
	DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("Elapsed Time :%.2f \n Jump Count :%d"),
	                                                                elapsedTime, jumpCounts),
	                nullptr,
	                FColor::Blue, 0, true, 1.0f);
}

#pragma region JumpStartRPC

void ANetWorkProject1Character::JumpStart()
{
	/*Jump();
	if(HasAuthority()) // ���������� ����
	{
	ServerJump_Implementation()
	}*/
	ServerJump(); //������ �ƴѰ������� ��û�ؼ� �������� ���� 
}


//������ ��û �� ��ȿ�� ��û������ ����
bool ANetWorkProject1Character::ServerJump_Validate() //������� WithValidation �߰��ϸ� �̷���  �߰� ���� �ؾ���
{
	return true;
	//return jumpCounts < 5;
	//false�Ǵ� ���� ���� ���� ����
	// return true ���� ������ ���� ������ �ϼ��Ǹ� �ֱ�  
}

void ANetWorkProject1Character::ServerJump_Implementation()
{
	jumpCounts++;
	repJumpCounts++;	// 
	MulticastJump();
	UE_LOG(LogTemp, Warning, TEXT("ServerJump_Called"));
	//NetMulticast,client �� ���  : ServerJump_Implementation ���ʿ��� ������Ѿ� ����� �۵� 
}

//��� Ŭ���̾�Ʈ���� ���ÿ� ������ ����
void ANetWorkProject1Character::MulticastJump_Implementation() //�̹� �������Լ��� �߰� 
{
	//UE_LOG(LogTemp,Warning,TEXT("MulticastJump__Called"));
	//��� Ŭ���̾�Ʈ���� ������� �ൿ�� ����
	Jump();
}

void ANetWorkProject1Character::OnRep_JumpEffect()
{
	UE_LOG(LogTemp,Warning,TEXT("OnRep_Jump__Called"));
	if(battleUI!=nullptr)
	{
		battleUI->PlayHitAnimation();
	}
}

#pragma endregion 
//////////////////////////////////////////////////////////////////////////
// Input


//SetupPlayerInputComponent �� �����ڿ� beginplay ���̿��� ���� 
void ANetWorkProject1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,
		                                   &ANetWorkProject1Character::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &ANetWorkProject1Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &ANetWorkProject1Character::Look);
		EnhancedInputComponent->BindAction(IA_ReleaseWeapon, ETriggerEvent::Started, this,
		                                   &ANetWorkProject1Character::ReleaseWeapon);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ANetWorkProject1Character::Fire);
		EnhancedInputComponent->BindAction(IA_VoiceControl, ETriggerEvent::Started, this, 
		&ANetWorkProject1Character::VoiceChatOn);
		EnhancedInputComponent->BindAction(IA_VoiceControl, ETriggerEvent::Completed, this, 
		&ANetWorkProject1Character::VoiceChatOff);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void ANetWorkProject1Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetWorkProject1Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANetWorkProject1Character::ReleaseWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("%s(%d) : release wepon"), *FString(__FUNCTION__), __LINE__);
	if (owningWeapon != nullptr)
	{
		owningWeapon->ReleaseWeapon(this);
		//owningWeapon->SetOwner(nullptr);
	}
}



#pragma region FireRPC



void ANetWorkProject1Character::Fire()
{
	if (owningWeapon != nullptr)
	{
		if(!GetWorldTimerManager().IsTimerActive(fireCoolTime))
		{
			// ������ �϶��� true���� ���ǹ� �Ҹ��� 
			//IsTimerActive �� ��������  (false)  �Լ��� ���� 
			ServerFire();
		}
		
	}
}




void ANetWorkProject1Character::ServerFire_Implementation()
{
	if (m_Ammo > 0)
	{
		owningWeapon->Fire(this); //������ �����ʿ��� *��* �ϵ��� ����  
		m_Ammo = FMath::Max(0, m_Ammo - 1); //�Ѿ��� ������ relicated  ���� = �������� �ٿ��� ��
	}
	MulticastFire();
}

void ANetWorkProject1Character::MulticastFire_Implementation()
{
	bool bHasAmmo = m_Ammo > 0;
	PlayAnimMontage(fireAnimMontage[(int32)bHasAmmo]);
	GetWorldTimerManager().SetTimer(fireCoolTime,m_attackDelay,false);
	//�������� Ÿ�̸Ӹ� �۵���Ŵ 
}	

#pragma endregion 


#pragma region DieRPC
void ANetWorkProject1Character::ServerDieProcess_Implementation()
{
	NetMulticastDieProcess();
}


void ANetWorkProject1Character::NetMulticastDieProcess_Implementation()
{
	// �Է¹��� -> ���ε���(SetupPlayerInputComponent) ���� �ѹ��� �����ǰ� , ���ε��� ����Ǿ��ִ� �Լ��� �������� ���ϵ��� 
	GetCharacterMovement()->DisableMovement(); //������, ������ ���� 
	bUseControllerRotationYaw=false;	// ȸ���� ���� 
	CameraBoom->bUsePawnControlRotation=false;
		
	// ���� �������´�
	ReleaseWeapon();
	//�浹 ó�� ��Ȱ��ȭ
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// ������ ȿ�� :   GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	
	if (pc !=nullptr && pc->IsLocalPlayerController())
		// Ư��Ŭ���̾�Ʈ (�� �Լ��� �����ϴ� �÷��̾ ������ ��쿡�� ���� =  (�״� Ŭ���̾�Ʈ�� �����ų �� )
		// Ư��Ŭ���̾�Ʈ�� �� (ȭ��) ������ �����ϰ� �� 
	{
		FTimerHandle dieEffectHandler;
		GetWorldTimerManager().SetTimer(dieEffectHandler,FTimerDelegate::CreateLambda([&]()
		{
		//���ǳ����� ��ư�� Ȱ��ȭ �ϱ�
		battleUI->ShowButtons();
		//Ŀ�����̵��� �ϱ�
		pc->SetShowMouseCursor(true);
		pc->SetInputMode(FInputModeUIOnly());
		//ȭ���� ������� �� ó���Ѵ�
		FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0,0,0,1.f);	
		}),1.1f,false);

		// APlayerController* pc ����������� Ÿ�̸Ӱ� ����ɶ��� �ְ� ->1.1�� ������ �� �����
		// APlayerController* pc �� ���������� ������ �Ѵ�!
	}
}

#pragma endregion 

#pragma region VoiceChat
void ANetWorkProject1Character::VoiceChatOn()
{
	pc->StartTalking();
}

void ANetWorkProject1Character::VoiceChatOff()
{
	pc->StopTalking();
	//pc->GetUniqueID();
	//pc->ServerMutePlayer(0);
}

#pragma endregion 

//�̹� �������̵� �Ȱ� = ����� ������ϰ� �׳� �����ͼ� ��븸�ϴ� ��
//
void ANetWorkProject1Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); //�θ𿡼� �������̵� �� �͵��� ����

	// DOREPLIFETIME(ANetWorkProject1Character,elapsedTime);
	// �������� ����ȭ tick �� ��ȭ���� ��� �ִ� ��
	// c :Ŭ���� v : ���� =>UPROPERTY(Replicated) �� ���־�� �������� �ν� 
	DOREPLIFETIME(ANetWorkProject1Character, jumpCounts);
	//DOREPLIFETIME_
	DOREPLIFETIME_CONDITION(ANetWorkProject1Character, elapsedTime, COND_AutonomousOnly);
	//���� �߰� COND_AutonomousOnly : �����Ȱ� �޴°Ϳ� ���� ����
	//������ �ڱ��ڽ��� Auto �ϱ� ��� ������ ??
	DOREPLIFETIME(ANetWorkProject1Character, owningWeapon);
	DOREPLIFETIME(ANetWorkProject1Character, m_Ammo);
	DOREPLIFETIME(ANetWorkProject1Character, m_damagePower);
	DOREPLIFETIME(ANetWorkProject1Character, m_attackDelay);
	DOREPLIFETIME(ANetWorkProject1Character, currentHealth);
	DOREPLIFETIME(ANetWorkProject1Character, repJumpCounts);
}
