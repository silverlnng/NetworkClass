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
#include "net/UnrealNetwork.h" //네트워크관련 헤더
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

	PrimaryActorTick.bCanEverTick = true; //틱사용 설정 여부 ! 이게 없으면 tick을 만들어도 실행안함

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
	//GetController()!=nullptr&&GetController()->IsLocalController() 으로 로컬플레이어  검사 
	if (battlewidget != nullptr && GetController() != nullptr && GetController()->IsLocalController())
	{
		battleUI = CreateWidget<UBattleWidget>(GetWorld(), battlewidget);
		if (battleUI != nullptr)
		{
			battleUI->AddToViewport(); //UI 는 자신의 UI 만 띄우기 
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

	if(!bIsDead && currentHealth <= 0 ) //캐릭터의 사망처리 tick 이어서 bool 조건없음 여러번 발생할수있음 
	{
		bIsDead=true;
		//죽는 애니메이션 실행 -> AnimInstance 변수값 -> 애니메이션 bp에서 변수로 조건 설정 
			
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
	//여기서는 Damaged() 가 서버에서만 발생해서 ServerDamaged_Implementation 으로도 가능하지만
	//확장성을 위해서(경우에 따라 로컬에서도 Damaged() 발생할 가능성이 있을수도 있음 ) ServerDamaged ->ServerDamaged_Implementation 과정을 거치기
	// ServerDamaged 는 서버 , 로컬 모두 실행가능
	//ServerDamaged_Implementation는 서버에서만 실행가능함 
}

void ANetWorkProject1Character::ServerDamaged_Implementation(int32 dmg)
{
	currentHealth = FMath::Max(0, currentHealth - dmg);
	// currentHealth 는 replicated 되는 변수이고 , 모두에게 동일하게 보여야한다 => 서버에서 실행
	//FMath::Max 는 둘중에 더큰값을 반환하는 함수
	
	ClientDamaged();
	//damaged 를  실행하는 클라이언트 "만" UI를 실행시키도록 함 
}

void ANetWorkProject1Character::ClientDamaged_Implementation()
{
	battleUI->PlayHitAnimation();
	
	//카메라 쉐이크 효과 를 주기
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
	//네트워그 role 확인용 로그
	FString localRoleString = *UEnum::GetValueAsString<ENetRole>(localRole);
	FString remoteRoleString = *UEnum::GetValueAsString<ENetRole>(remoteRole);
	FString ownerString = GetOwner() == nullptr ? *FString("No owner") : *GetOwner()->GetActorNameOrLabel();

	FString connectionString = GetNetConnection() == nullptr
		                           ? *FString("Invalid Connection")
		                           : *FString("Valid Connection");

	FString printString = FString::Printf(
		TEXT("LocalRole : %s \n remote Role : %s \n owner : %s \n connection: %s \n "), *localRoleString,
		*remoteRoleString, *ownerString, *connectionString);

	// 여기서 * 는 포인터가 아님 : 연산자 오버라이드 * 표시 => tchar형태로 반환된것
	// UEnum::GetValueAsString<ENetRole> : 값에 해당하는 enum값을 문자열로 
	DrawDebugString(GetWorld(), GetActorLocation(), printString, nullptr, FColor::White, 0, true, 1.0f);
	*/

	//게임프레임 워크 확인용 로그
	FString gameModeString = GetWorld()->GetAuthGameMode()!=nullptr ? *FString("validGameMode"):*FString("invalidGameMode"); 
	//GetAuthGameMode : 현재 월드에 해당하는 게임모드 
	//월드마다 게임모드 세팅하니까(override ) , 없는경우 디폴트 값 GameMode 사용

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
	//GetLocalRole() == ROLE_Authority 서버에서만 변경하면된다
	//서버에서 GetLocalRole 은 모두 ROLE_Authority
	// 로컬에서 GetLocalRole() == ROLE_Authority 없음 
	if (GetLocalRole() == ROLE_Authority)
	{
		elapsedTime += deltaSeconds;
	}

	//조건이없을때 클라이언트도 lapsedTime 을 보내지만 서버의 elapsedTime으로 덮어쓰기 되는 것 
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
	if(HasAuthority()) // 서버에서만 실행
	{
	ServerJump_Implementation()
	}*/
	ServerJump(); //서버가 아닌곳에서도 요청해서 서버에서 실행 
}


//서버에 요청 시 유효한 요청인지를 검증
bool ANetWorkProject1Character::ServerJump_Validate() //헤더에서 WithValidation 추가하면 이렇게  추가 정의 해야함
{
	return true;
	//return jumpCounts < 5;
	//false되는 순간 세션 강제 종료
	// return true 으로 제작한 다음 조건이 완성되면 넣기  
}

void ANetWorkProject1Character::ServerJump_Implementation()
{
	jumpCounts++;
	repJumpCounts++;	// 
	MulticastJump();
	UE_LOG(LogTemp, Warning, TEXT("ServerJump_Called"));
	//NetMulticast,client 의 경우  : ServerJump_Implementation 안쪽에서 실행시켜야 제대로 작동 
}

//모든 클라이언트에서 동시에 실행할 내용
void ANetWorkProject1Character::MulticastJump_Implementation() //이미 구현된함수에 추가 
{
	//UE_LOG(LogTemp,Warning,TEXT("MulticastJump__Called"));
	//모든 클라이언트에게 점프라는 행동을 실행
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


//SetupPlayerInputComponent 는 생성자와 beginplay 사이에서 실행 
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
			// 실행중 일때는 true으로 조건문 불만족 
			//IsTimerActive 가 끝났을때  (false)  함수를 실행 
			ServerFire();
		}
		
	}
}




void ANetWorkProject1Character::ServerFire_Implementation()
{
	if (m_Ammo > 0)
	{
		owningWeapon->Fire(this); //생성은 서버쪽에서 *만* 하도록 설정  
		m_Ammo = FMath::Max(0, m_Ammo - 1); //총알의 갯수는 relicated  변수 = 서버에서 줄여야 함
	}
	MulticastFire();
}

void ANetWorkProject1Character::MulticastFire_Implementation()
{
	bool bHasAmmo = m_Ammo > 0;
	PlayAnimMontage(fireAnimMontage[(int32)bHasAmmo]);
	GetWorldTimerManager().SetTimer(fireCoolTime,m_attackDelay,false);
	//서버에서 타이머를 작동시킴 
}	

#pragma endregion 


#pragma region DieRPC
void ANetWorkProject1Character::ServerDieProcess_Implementation()
{
	NetMulticastDieProcess();
}


void ANetWorkProject1Character::NetMulticastDieProcess_Implementation()
{
	// 입력방지 -> 바인딩은(SetupPlayerInputComponent) 최초 한번에 설정되고 , 바인딩에 연결되어있는 함수를 실행하지 못하도록 
	GetCharacterMovement()->DisableMovement(); //움직임, 점프를 막음 
	bUseControllerRotationYaw=false;	// 회전을 막음 
	CameraBoom->bUsePawnControlRotation=false;
		
	// 총을 내려놓는다
	ReleaseWeapon();
	//충돌 처리 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 동일한 효과 :   GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	
	if (pc !=nullptr && pc->IsLocalPlayerController())
		// 특정클라이언트 (이 함수를 실행하는 플레이어가 로컬일 경우에만 실행 =  (죽는 클라이언트만 실행시킬 것 )
		// 특정클라이언트의 방 (화면) 에서만 실행하게 됨 
	{
		FTimerHandle dieEffectHandler;
		GetWorldTimerManager().SetTimer(dieEffectHandler,FTimerDelegate::CreateLambda([&]()
		{
		//세션나가기 버튼을 활성화 하기
		battleUI->ShowButtons();
		//커서보이도록 하기
		pc->SetShowMouseCursor(true);
		pc->SetInputMode(FInputModeUIOnly());
		//화면을 흑백으로 후 처리한다
		FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0,0,0,1.f);	
		}),1.1f,false);

		// APlayerController* pc 지역변수라면 타이머가 실행될때만 있고 ->1.1초 지나서 는 사라짐
		// APlayerController* pc 를 전역변수로 만들어야 한다!
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

//이미 오버라이드 된것 = 헤더에 선언안하고 그냥 가져와서 사용만하는 것
//
void ANetWorkProject1Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); //부모에서 오버라이드 된 것들을 실행

	// DOREPLIFETIME(ANetWorkProject1Character,elapsedTime);
	// 서버만의 동기화 tick 에 변화값을 계속 주는 것
	// c :클래스 v : 변수 =>UPROPERTY(Replicated) 가 되있어야 서버에서 인식 
	DOREPLIFETIME(ANetWorkProject1Character, jumpCounts);
	//DOREPLIFETIME_
	DOREPLIFETIME_CONDITION(ANetWorkProject1Character, elapsedTime, COND_AutonomousOnly);
	//조건 추가 COND_AutonomousOnly : 복제된걸 받는것에 대한 조건
	//서버에 자기자신은 Auto 니까 모두 전송함 ??
	DOREPLIFETIME(ANetWorkProject1Character, owningWeapon);
	DOREPLIFETIME(ANetWorkProject1Character, m_Ammo);
	DOREPLIFETIME(ANetWorkProject1Character, m_damagePower);
	DOREPLIFETIME(ANetWorkProject1Character, m_attackDelay);
	DOREPLIFETIME(ANetWorkProject1Character, currentHealth);
	DOREPLIFETIME(ANetWorkProject1Character, repJumpCounts);
}
