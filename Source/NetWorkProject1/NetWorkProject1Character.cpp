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
#include "net/UnrealNetwork.h"	
//네트워크관련 헤더
#include "PistolActor.h"
#include "BattleWidget.h"

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
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	PrimaryActorTick.bCanEverTick = true;	//틱사용 설정 여부 ! 이게 없으면 tick을 만들어도 실행안함

	bReplicates =true;
	SetReplicateMovement(true);
}

void ANetWorkProject1Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	localRole = GetLocalRole();
	remoteRole = GetRemoteRole();

	//GetController()!=nullptr&&GetController()->IsLocalController() 으로 검사 
	if(battlewidget!=nullptr && GetController()!=nullptr&&GetController()->IsLocalController())
	{
		battleUI = CreateWidget<UBattleWidget>(GetWorld(),battlewidget);
		if(battleUI!=nullptr)
		{
			battleUI->AddToViewport();	//UI 는 자신의 UI 만 띄우기 
		}
	}
}

void ANetWorkProject1Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//PrintInfoLog();
	//PrintTimeLog(DeltaSeconds);
	
}

void ANetWorkProject1Character::setWeaponInfo(int32 ammo, float damage, float delay)
{
	m_Ammo=ammo;
	m_damagePower=damage;
	m_attackDelay=delay;
}

void ANetWorkProject1Character::PrintInfoLog()
{
	FString localRoleString = *UEnum::GetValueAsString<ENetRole>(localRole);
	FString remoteRoleString = *UEnum::GetValueAsString<ENetRole>(remoteRole);
	FString ownerString = GetOwner() == nullptr ? *FString("No owner") : *GetOwner()->GetActorNameOrLabel();
	
	FString connectionString = GetNetConnection() ==nullptr? *FString("Invalid Connection") : *FString("Valid Connection");
		
	FString printString = FString::Printf(TEXT("LocalRole : %s \n remote Role : %s \n owner : %s \n connection: %s \n "),*localRoleString,*remoteRoleString,*ownerString,*connectionString);
	
	// 여기서 * 는 포인터가 아님 : 연산자 오버라이드 * 표시 => tchar형태로 반환된것
	// UEnum::GetValueAsString<ENetRole> : 값에 해당하는 enum값을 문자열로 
	DrawDebugString(GetWorld(),GetActorLocation(),printString,nullptr,FColor::White,0,true,1.0f);
}

void ANetWorkProject1Character::PrintTimeLog(float deltaSeconds)
{
	//GetLocalRole() == ROLE_Authority 서버에서만 변경하면된다
	//서버에서 GetLocalRole 은 모두 ROLE_Authority
	// 로컬에서 GetLocalRole() == ROLE_Authority 없음 
	if(GetLocalRole() == ROLE_Authority)
	{
	 elapsedTime+=deltaSeconds;
	}

	//조건이없을때 클라이언트도 lapsedTime 을 보내지만 서버의 elapsedTime으로 덮어쓰기 되는 것 
	// 
	//UE_LOG(LogTemp,Warning,TEXT("Elapsed Time :%.2f"),elapsedTime);
	DrawDebugString(GetWorld(),GetActorLocation(),FString::Printf(TEXT("Elapsed Time :%.2f \n Jump Count :%d"),
	elapsedTime,jumpCounts),
	nullptr,
	FColor::Blue,0,true,1.0f);
}

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
	return  jumpCounts <5;
	//false되는 순간 세션 강제 종료
	// return true 으로 제작한 다음 조건이 완성되면 넣기  
}

void ANetWorkProject1Character::ServerJump_Implementation()
{
	jumpCounts++;
	MulticastJump();
	UE_LOG(LogTemp,Warning,TEXT("ServerJump_Called"));
	//NetMulticast,client 의 경우  : ServerJump_Implementation 안쪽에서 실행시켜야 제대로 작동 
}

//모든 클라이언트에서 동시에 실행할 내용
void ANetWorkProject1Character::MulticastJump_Implementation() //이미 구현된함수에 추가 
{
	UE_LOG(LogTemp,Warning,TEXT("MulticastJump__Called"));
	//모든 클라이언트에게 점프라는 행동을 실행
	Jump();
}



//////////////////////////////////////////////////////////////////////////
// Input

void ANetWorkProject1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANetWorkProject1Character::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetWorkProject1Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetWorkProject1Character::Look);
		EnhancedInputComponent->BindAction(IA_ReleaseWeapon, ETriggerEvent::Started, this, &ANetWorkProject1Character::ReleaseWeapon);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ANetWorkProject1Character::Fire);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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

void ANetWorkProject1Character::ReleaseWeapon(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Warning,TEXT("%s(%d) : release wepon"),*FString(__FUNCTION__),__LINE__);
	if(Value.Get<bool>())
	{
		if (owningWeapon != nullptr)
		{
			owningWeapon->ReleaseWeapon(this);
			//owningWeapon->SetOwner(nullptr);
		}
	}
}


//이미 오버라이드 된것 = 헤더에 선언안하고 그냥 가져와서 사용만하는 것
//
void ANetWorkProject1Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	//부모에서 오버라이드 된 것들을 실행

	// DOREPLIFETIME(ANetWorkProject1Character,elapsedTime);
	// 서버만의 동기화 tick 에 변화값을 계속 주는 것
	// c :클래스 v : 변수 =>UPROPERTY(Replicated) 가 되있어야 서버에서 인식 
	DOREPLIFETIME(ANetWorkProject1Character,jumpCounts);
	//DOREPLIFETIME_
	DOREPLIFETIME_CONDITION(ANetWorkProject1Character,elapsedTime,COND_AutonomousOnly);
	//조건 추가 COND_AutonomousOnly : 복제된걸 받는것에 대한 조건
	//서버에 자기자신은 Auto 니까 모두 전송함 ??
	DOREPLIFETIME(ANetWorkProject1Character,owningWeapon);
	DOREPLIFETIME(ANetWorkProject1Character,m_Ammo);
	DOREPLIFETIME(ANetWorkProject1Character,m_damagePower);
	DOREPLIFETIME(ANetWorkProject1Character,m_attackDelay);
}


void ANetWorkProject1Character::Fire()
{
	if (owningWeapon != nullptr)
	{
		ServerFire();
		//총알의 갯수는 relicated  변수 = 서버에서 줄여야 함
	}
}

void ANetWorkProject1Character::MulticastFire_Implementation()
{
	//남아있는 총알 검사 하기 
	bool bHasAmmo = m_Ammo>0;
	//bHasAmmo = true 이면 1 , false 이면 0 을  
	PlayAnimMontage(fireAnimMontage[(int32)bHasAmmo]);
}

void ANetWorkProject1Character::ServerFire_Implementation()
{
	m_Ammo=FMath::Max(0,m_Ammo-1);
	MulticastFire();
}
