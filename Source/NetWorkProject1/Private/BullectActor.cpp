// Fill out your copyright notice in the Description page of Project Settings.


#include "BullectActor.h"

#include "NetPlayerState.h"
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
	SetRootComponent(sphereComp); //�ϸ� sphereComp = RootComponent
	
	sphereComp->SetSphereRadius(50.f); // 1m ¥�� �� .1 = 1cm
	
	sphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	sphereComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Ignore);
	//���� collision channel : ECC_GameTraceChannel1 
	sphereComp->SetWorldScale3D(FVector(0.2f));
	sphereComp->SetSimulatePhysics(false);
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Physics ���������� ���������ϱ� QueryOnly ���� �������� �������� �����Ѱ� .
	// QueryOnly : �浹ü�� ���,ũ�⿡ ���� ����� ��� �� �޾ƿ� 
	// overlap �� �Ÿ����� ����� ���
		
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(5.f); //�Ѿ��� ���� �ֱ� . �����ð������� �˾Ƽ� ����� 
	
	bReplicates = true; //����� #include  "Net/UnrealNetwork.h" �߰� 
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
		//������ HasAuthority ���Ѱ˻�� ���������� �Ѿ��� ������ �ִ� ����  
	if(HasAuthority())
	{
		if(ANetWorkProject1Character* player =Cast<ANetWorkProject1Character>(OtherActor))
		{
			//player ->�Ѿ˿� �����÷��̾�  
			player->Damaged(damage);

			//�Ѿ��� ���� �÷��̾� (GetOwner()) �� PlayerState �� score ������ ���� 10�� �߰�
			ANetPlayerState* ownerPlayer= Cast<ANetWorkProject1Character>(GetOwner())->GetPlayerState<ANetPlayerState>();
			ownerPlayer->SetScore(ownerPlayer->GetScore()+10);
		}
	 	
	  Destroy();
	}
}

