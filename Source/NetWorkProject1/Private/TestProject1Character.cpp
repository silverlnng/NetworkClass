// Fill out your copyright notice in the Description page of Project Settings.


#include "TestProject1Character.h"

#include "KismetTraceUtils.h"


ATestProject1Character::ATestProject1Character()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATestProject1Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//InteractionPositive();
}

void ATestProject1Character::InteractionPositive()
{
	FVector _location;
	FRotator _rotation;
	FHitResult _hitOut;
	TArray<FHitResult> _HitOutArray = {};
	
	GetController()->GetPlayerViewPoint(_location,_rotation);
	FVector _start = _location;
	FVector _End = (_rotation.Vector()*1500);

	FCollisionQueryParams _traceParams;
	_traceParams.AddIgnoredActor(this);
	
	//bool bisHit = GetWorld()->LineTraceMultiByChannel(_HitOutArray,_start,_End,ECC_Visibility,_traceParams);
	//UKismetSystemLibrary::BoxTraceSingle()

	bool bisSweep = GetWorld()->SweepMultiByChannel(_HitOutArray,_start,_End,FQuat::Identity,ECC_GameTraceChannel1,
	FCollisionShape::MakeBox(FVector(50)),_traceParams);

	if(bisSweep)
	{
		for(const FHitResult& hitInfos:_HitOutArray)
		{
			// 드로우할 박스의 위치는 hit 점과 시작점 중간점
			// 드로우할 박스의 길이를 시작점과 hit 점사이의 거리 *0.5
			// *** 주의사항 DrawDebugBox 의 extent 는 드로우할 박스의 절반 
			//FVector boxlength = FVector(FMath::Abs(hitInfos.ImpactPoint.X-_start.X),FMath::Abs(hitInfos.ImpactPoint.Y-_start.Y),FMath::Abs(hitInfos.ImpactPoint.Z-_start.Z));
			FVector boxlength = FVector(FMath::Abs(hitInfos.ImpactPoint-_start))*0.5f;
			
			DrawDebugBox(GetWorld(),(_start+hitInfos.ImpactPoint)*0.5f,boxlength,FColor::Black,0,0,0,1.5f);
		}
		
	}
	/*if(bisHit)
	{
	 DrawDebugLine(GetWorld(),_start,_End,FColor::Cyan,false,2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(),_start,_End,FColor::Red,false,2.0f);
	}*/

	/*
	if(!_HitOutArray.IsEmpty())
	{
		FString firstDetectActorName = *_HitOutArray[0].GetActor()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s(%d) : %s"), *FString(__FUNCTION__), __LINE__,*firstDetectActorName);
	}
	*/
	
	
}


void ATestProject1Character::SensorUsingOvelap()
{
	TArray<FOverlapResult> hitResults;
	FVector centorLoc = GetActorLocation();
	FQuat playerRot = GetActorQuat();
	
	FCollisionQueryParams _traceParams;
	_traceParams.AddIgnoredActor(this);
	
	//언리얼에서 오일러 형식으로 써도 내부적으로 쿼터니온 방식으로 회전
	bool bIsHit = GetWorld()->OverlapMultiByChannel(hitResults,centorLoc,playerRot,ECC_GameTraceChannel1,
	FCollisionShape::MakeSphere(1500),_traceParams);
	if(bIsHit)
	{
		for(const FOverlapResult hitResult :hitResults )
		{
			UE_LOG(LogTemp,Warning,TEXT("%s"),*hitResult.GetActor()->GetActorNameOrLabel());
			DrawDebugSphere(GetWorld(),hitResult.GetActor()->GetActorLocation(),50,20,FColor::Red,false,0,0,1.5f);
		}
	}
}


