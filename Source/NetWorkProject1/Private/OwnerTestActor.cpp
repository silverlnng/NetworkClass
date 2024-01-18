// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnerTestActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetWorkProject1/NetWorkProject1Character.h"
#include "EngineUtils.h"
#include "IntVectorTypes.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AOwnerTestActor::AOwnerTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	rootComp =  CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootComp);

	meshComp =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	meshComp->SetupAttachment(rootComp);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AOwnerTestActor::BeginPlay()
{
	Super::BeginPlay();
	localRole = GetLocalRole();
	remoteRole = GetRemoteRole();
}

// Called every frame
void AOwnerTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//PrintInfoLog();
	
	AActor* owningActor = GetNearestDistance();
	/*UE_LOG(LogTemp,Warning,TEXT("Nearest is %s"),owningActor!=nullptr ? *owningActor->GetActorNameOrLabel():*FString
	("No Owner"));*/
	DrawDebugSphere(GetWorld(),GetActorLocation(),maxDistance,30,FColor::Green,false,0,0,1.0f);
}



void AOwnerTestActor::PrintInfoLog()
{
	FString characterName = GetActorNameOrLabel();
	FString localRoleString = *UEnum::GetValueAsString<ENetRole>(localRole);
	FString remoteRoleString = *UEnum::GetValueAsString<ENetRole>(remoteRole);
	FString ownerString = GetOwner() == nullptr ? *FString("No owner") : *GetOwner()->GetActorNameOrLabel();
	
	FString connectionString = GetNetConnection() ==nullptr? *FString("Invalid Connection") : *FString("Valid Connection");
		
	FString printString = FString::Printf(TEXT("playerName: %s \n LocalRole : %s \n remote Role : %s \n owner : %s \n connection: %s \n "),*characterName,*localRoleString,*remoteRoleString,*ownerString,*connectionString);
	
	// 여기서 * 는 포인터가 아님 : 연산자 오버라이드 * 표시 => tchar형태로 반환된것
	// UEnum::GetValueAsString<ENetRole> : 값에 해당하는 enum값을 문자열로 
	DrawDebugString(GetWorld(),GetActorLocation(),printString,nullptr,FColor::Yellow,0,true,1.0f);
}

AActor* AOwnerTestActor::GetNearestDistance()
{
	/*TArray<float> lengthArray;
	
	// 최대거리
	// 플레이어 검색하기
	// TActorIterator 는 순서를 지정못함 .자료형의 일종으로 생각하기
	// 가운데가 ' player ==null ' 조건이 생략되어 있는 상태  
	for (TActorIterator<ANetWorkProject1Character> player(GetWorld()); player; ++player)
	{
		FVector dist = player->GetActorLocation()-this->GetActorLocation();
		float Length = dist.Length();
		if (Length<maxDistance)
		{
			lengthArray.Push(Length);
			//거리값들을 배열에 넣은다음 최소 값 구하기
			//push 를 계속하는게 아니라 갱신하도록 만들어보기 ! ==> 전역변수로 TArray로 만들어서 계속 누적이 된것 !
			//지역변수로 정의해서 한번 실행 끝 -> 재실행 다시 array 에 넣기 
		}
	}
	float min;
	if(!lengthArray.IsEmpty())
	{
		min = lengthArray[0];
		for(int i=0 ; i<lengthArray.Num() ; i++)
		{
			if(min>lengthArray[i])
			{
				min = lengthArray[i];
			}
		}
	}
	
	return lengthArray.IsEmpty()? maxDistance : min ;*/

	//최적화 코드
#pragma region blueprint Type
	/*TArray<ANetWorkProject1Character*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ANetWorkProject1Character::StaticClass(),players);*/
#pragma endregion

#pragma region C++ Type
	float minDistance =maxDistance;
	AActor* nearestActor = nullptr;
	for (TActorIterator<ANetWorkProject1Character> player(GetWorld()); player; ++player)
	{
		ANetWorkProject1Character* cha = *player;	//포인터 사용하기 
		float curDistance = FVector::Distance(cha->GetActorLocation(),GetActorLocation());
		if(curDistance<minDistance)
		{
			minDistance=curDistance;
			//minDistance 가 계속해서 작은값 (최소값) 으로 갱신됨
			nearestActor=cha;
		}
	}

	//return minDistance;	//curDistance가 maxDistance 보다 크다면 한번도 if문을 돌지않아서 그대로 초기값maxDistance 반환
	if(nearestActor!=GetOwner())
	{
		SetOwner(nearestActor);
	}
	return nearestActor;
#pragma endregion	
}


