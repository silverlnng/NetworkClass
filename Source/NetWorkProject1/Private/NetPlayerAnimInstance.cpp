// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "../NetWorkProject1Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//NativeInitializeAnimation : �ִϸ��̼��� beginPlay
	
	player = Cast<ANetWorkProject1Character>(GetOwningActor()) ;

	//GetOwningActor �� ���ø��� �����ϱ� ĳ������ �ؼ� �������°� !
	
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
	
	if(velocity.IsNearlyZero()) //�������� ���������� 
	{
		speed=0;
	}
	else
	{
		speed = velocity.Size2D();	//z���� ������ �ӵ��� ũ�Ⱚ�� ������Ʈ�� �޾ƿ� 
	}
	degree = GetDegreeOfVelocity(velocity,player->GetActorForwardVector(),player->GetActorRightVector());
	// ������ ���Ⱚ �ӵ��� ��������
	// ������ ���� -> �ڻ����� ���Լ��� ������  �˾ƿ��� -> ���Ȱ����� ���ͼ� �ٽ� �����κ�ȯ���ֱ�

	// �÷��̾��� �� �������� Ȯ��
	bHasPistol = player->GetOwningWeapon() !=nullptr;

	bIsDead = player->GetDeadState();
}

float UNetPlayerAnimInstance::GetDegreeOfVelocity(FVector& velocity, FVector forwardVec, FVector rightVec)
{
	float angle = 0;
	//UE_LOG(LogTemp,Warning,TEXT(" ") ,)
	//���� �Ű������� ������ velocity�� ���� 0 �̸� ���ʿ� �������� �ʴ°�
	//�ӵ� ���Ͱ� ���� 0�������� ��������� �����ʴ´� 
	if (!velocity.IsNearlyZero())
	{
		FVector normalVelocity = velocity.GetSafeNormal();
 		
		float theta = FMath::Acos(FVector::DotProduct(normalVelocity,forwardVec)) ;
		
		angle = FMath::RadiansToDegrees(theta);
		//���� �� ������ ��ȯ�ϱ�
		float cosT = FVector::DotProduct(normalVelocity,rightVec);
		if(cosT<0)
		{
			angle*=-1.0f;
		}
	}
		return angle;
}


