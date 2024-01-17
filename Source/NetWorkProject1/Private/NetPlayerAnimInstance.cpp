// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "../NetWorkProject1Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//NativeInitializeAnimation : 애니메이션의 beginPlay
	
	player = Cast<ANetWorkProject1Character>(GetOwningActor()) ;

	//GetOwningActor 에 템플릿이 없으니까 캐스팅을 해서 가져오는것 !
	
	if(player!=nullptr)
	{
		playerMovement = player->GetCharacterMovement();
	}
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(player==nullptr || playerMovement ==nullptr) return;

	FVector velocity =playerMovement->Velocity;
	
	if(velocity.IsNearlyZero()) //허용오차를 넣을수있음 
	{
		speed=0;
	}
	else
	{
		speed = velocity.Size2D();	//z값을 제외한 속도의 크기값을 업데이트로 받아옴 
	}
	degree = GetDegreeOfVelocity(velocity,player->GetActorForwardVector(),player->GetActorRightVector());
	// 현재의 방향값 속도값 가져오기
	// 벡터의 내적 -> 코사인의 역함수로 각도값  알아오기 -> 라디안값으로 나와서 다시 각도로변환해주기

	// 플레이어의 총 소유여부 확인
	bHasPistol = player->GetOwningWeapon() !=nullptr;

	bIsDead = player->GetDeadState();
}

float UNetPlayerAnimInstance::GetDegreeOfVelocity(FVector& velocity, FVector forwardVec, FVector rightVec)
{
	float angle = 0;
	//UE_LOG(LogTemp,Warning,TEXT(" ") ,)
	//만약 매개변수로 들어오는 velocity이 거의 0 이면 애초에 움직이지 않는것
	//속도 벡터가 거의 0에가까우면 각도계산을 하지않는다 
	if (!velocity.IsNearlyZero())
	{
		FVector normalVelocity = velocity.GetSafeNormal();
 		
		float theta = FMath::Acos(FVector::DotProduct(normalVelocity,forwardVec)) ;
		
		angle = FMath::RadiansToDegrees(theta);
		//라디안 을 각도로 변환하기
		float cosT = FVector::DotProduct(normalVelocity,rightVec);
		if(cosT<0)
		{
			angle*=-1.0f;
		}
	}
		return angle;
}


