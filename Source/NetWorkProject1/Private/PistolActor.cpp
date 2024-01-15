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

//OnOverlap 은 특성상 클라이언트 , 서버 모두에서 실행
void APistolActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//플레이어들과 충돌처리
	//소켓에 붙이고 싶다 => 소켓은 컴포넌트에 있다 => attach to component 으로 해야한다
	ANetWorkProject1Character* player =Cast<ANetWorkProject1Character>(OtherActor);

	// 나 자신(총) 도 주인이 없는 상태 
	if(player!=nullptr && player->GetOwningWeapon()==nullptr && GetOwner()==nullptr)
	{
		
		GrabPistol(player);
		//UE_LOG(LogTemp,Warning,TEXT("Pistol Collide Player"));
	}
}

void APistolActor::GrabPistol(class ANetWorkProject1Character* player)
{
	//playercontroller가 possess 되지않는 액터는 owner가 있어야 rpc를 사용할수 있다
	
	if(HasAuthority()) //서버의 경우
	{
		SetOwner(player);	//나의 오너를 플레이어로 설정 
		ServerGrabPistol_Implementation(player);
	}
	/*else if(player->GetController() && player->GetController()->IsLocalController())
	{
		//로컬 컨트롤러일 경우에만 서버 rpc 함수를 실행
		ServerGrabPistol(player);	
	}*/
		
}

void APistolActor::ReleaseWeapon(class ANetWorkProject1Character* player)
{
	if(player==nullptr) {return;}
	// 분리하기 
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	// 다시 물리 , 충돌 작용할수있도록 되돌리기
	boxComp->SetSimulatePhysics(true);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// 놓으니까 플레이어의 owningWeapon 변수를 null 으로
	player->SetOwningWeapon(nullptr);

	// 나 자신(pistol)의 owner도 null으로 만들기 : 통신때문에 제일 마지막에 하기
	//SetOwner(nullptr);
}

void APistolActor::ServerGrabPistol_Implementation(ANetWorkProject1Character* player)
{
	player->SetOwningWeapon(this);	//서버에서만
	MulticastGrabPistol(player);
}

void APistolActor::MulticastGrabPistol_Implementation(ANetWorkProject1Character* player)
{
	boxComp->SetSimulatePhysics(false);
	//AttachToComponent를 하기 위해서 SetSimulatePhysics 가 해제되어있어야함  
	AttachToComponent(player->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("PistolSocket"));
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



