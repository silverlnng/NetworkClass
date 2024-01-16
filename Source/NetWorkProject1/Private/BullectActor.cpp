// Fill out your copyright notice in the Description page of Project Settings.


#include "BullectActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include  "Net/UnrealNetwork.h"
#include "NetWorkProject1/NetWorkProject1Character.h"
// Sets default values
ABullectActor::ABullectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(sphereComp); //하면 sphereComp = RootComponent
	
	sphereComp->SetSphereRadius(50.f); // 1m 짜리 구 .1 = 1cm
	
	sphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	sphereComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Ignore);
	//총의 collision channel : ECC_GameTraceChannel1 
	sphereComp->SetWorldScale3D(FVector(0.2f));
	sphereComp->SetSimulatePhysics(false);
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Physics 물리설정을 해제했으니까 QueryOnly 으로 반응값을 수식으로 설정한것 .
	// QueryOnly : 충돌체의 모양,크기에 따라서 계산한 결과 를 받아옴 
	// overlap 은 거리값을 계산한 결과
		
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(5.f);
	
	bReplicates = true; //헤더에 #include  "Net/UnrealNetwork.h" 추가 
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABullectActor::BeginPlay()
{
	Super::BeginPlay();
	sphereComp->OnComponentBeginOverlap.AddDynamic(this,&ABullectActor::OnOverlap);
}

// Called every frame
void ABullectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(HasAuthority())
	{
	SetActorLocation(GetActorLocation()+GetActorForwardVector()*moveSpeed*DeltaTime);
	}
	
}

void ABullectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("hit"));
	if(HasAuthority())
	{
		//생성도 HasAuthority 권한검사로 서버에서만 총알을 가지고 있는 상태  
		if(ANetWorkProject1Character* player =Cast<ANetWorkProject1Character>(OtherActor))
		{
			player->Damaged(damage);
		}
	 	
	  Destroy();
	}
}

