// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetWorkProject1/NetWorkProject1Character.h"

// Sets default values
APistolActor::APistolActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxComp->SetGenerateOverlapEvents(true);
	boxComp->SetSimulatePhysics(true);
	boxComp->SetEnableGravity(true);
	boxComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	boxComp->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	//캐릭터의 콜리전 설정이 pawn 
	//캐릭터와 피스톨 의 감지는 overlap으로 하기 
	
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APistolActor::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this,&APistolActor::OnOverlap);
}

// Called every frame
void APistolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistolActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//플레이어들과 충돌처리
	//소켓에 붙이고 싶다 => 소켓은 컴포넌트에 있다 => attach to component 으로 해야한다
	ANetWorkProject1Character* player =Cast<ANetWorkProject1Character>(OtherActor);

	// 나 자신(총) 도 주인이 없는 상태 
	if(player!=nullptr && player->GetOwningWeapon()==nullptr && GetOwner()==nullptr)
	{
		boxComp->SetSimulatePhysics(false);
		//AttachToComponent를 하기 위해서 SetSimulatePhysics 가 해제되어있어야함  
		AttachToComponent(player->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("PistolSocket"));

		if(HasAuthority())
		{
			
		SetOwner(player);
		//나의 오너를 플레이어로 설정 
		//boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		player->SetOwningWeapon(this);
		//플레이어에게도 플레이어의 소유변수로 자신(총) 으로 설정한다
		}
		
		//UE_LOG(LogTemp,Warning,TEXT("Pistol Collide Player"));
	}
}


