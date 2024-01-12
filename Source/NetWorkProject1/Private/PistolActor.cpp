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
	//ĳ������ �ݸ��� ������ pawn 
	//ĳ���Ϳ� �ǽ��� �� ������ overlap���� �ϱ� 
	
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
	//�÷��̾��� �浹ó��
	//���Ͽ� ���̰� �ʹ� => ������ ������Ʈ�� �ִ� => attach to component ���� �ؾ��Ѵ�
	ANetWorkProject1Character* player =Cast<ANetWorkProject1Character>(OtherActor);

	// �� �ڽ�(��) �� ������ ���� ���� 
	if(player!=nullptr && player->GetOwningWeapon()==nullptr && GetOwner()==nullptr)
	{
		boxComp->SetSimulatePhysics(false);
		//AttachToComponent�� �ϱ� ���ؼ� SetSimulatePhysics �� �����Ǿ��־����  
		AttachToComponent(player->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("PistolSocket"));

		if(HasAuthority())
		{
			
		SetOwner(player);
		//���� ���ʸ� �÷��̾�� ���� 
		//boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		player->SetOwningWeapon(this);
		//�÷��̾�Ե� �÷��̾��� ���������� �ڽ�(��) ���� �����Ѵ�
		}
		
		//UE_LOG(LogTemp,Warning,TEXT("Pistol Collide Player"));
	}
}


