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
	
	// ���⼭ * �� �����Ͱ� �ƴ� : ������ �������̵� * ǥ�� => tchar���·� ��ȯ�Ȱ�
	// UEnum::GetValueAsString<ENetRole> : ���� �ش��ϴ� enum���� ���ڿ��� 
	DrawDebugString(GetWorld(),GetActorLocation(),printString,nullptr,FColor::Yellow,0,true,1.0f);
}

AActor* AOwnerTestActor::GetNearestDistance()
{
	/*TArray<float> lengthArray;
	
	// �ִ�Ÿ�
	// �÷��̾� �˻��ϱ�
	// TActorIterator �� ������ �������� .�ڷ����� �������� �����ϱ�
	// ����� ' player ==null ' ������ �����Ǿ� �ִ� ����  
	for (TActorIterator<ANetWorkProject1Character> player(GetWorld()); player; ++player)
	{
		FVector dist = player->GetActorLocation()-this->GetActorLocation();
		float Length = dist.Length();
		if (Length<maxDistance)
		{
			lengthArray.Push(Length);
			//�Ÿ������� �迭�� �������� �ּ� �� ���ϱ�
			//push �� ����ϴ°� �ƴ϶� �����ϵ��� ������ ! ==> ���������� TArray�� ���� ��� ������ �Ȱ� !
			//���������� �����ؼ� �ѹ� ���� �� -> ����� �ٽ� array �� �ֱ� 
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

	//����ȭ �ڵ�
#pragma region blueprint Type
	/*TArray<ANetWorkProject1Character*> players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ANetWorkProject1Character::StaticClass(),players);*/
#pragma endregion

#pragma region C++ Type
	float minDistance =maxDistance;
	AActor* nearestActor = nullptr;
	for (TActorIterator<ANetWorkProject1Character> player(GetWorld()); player; ++player)
	{
		ANetWorkProject1Character* cha = *player;	//������ ����ϱ� 
		float curDistance = FVector::Distance(cha->GetActorLocation(),GetActorLocation());
		if(curDistance<minDistance)
		{
			minDistance=curDistance;
			//minDistance �� ����ؼ� ������ (�ּҰ�) ���� ���ŵ�
			nearestActor=cha;
		}
	}

	//return minDistance;	//curDistance�� maxDistance ���� ũ�ٸ� �ѹ��� if���� �����ʾƼ� �״�� �ʱⰪmaxDistance ��ȯ
	if(nearestActor!=GetOwner())
	{
		SetOwner(nearestActor);
	}
	return nearestActor;
#pragma endregion	
}


