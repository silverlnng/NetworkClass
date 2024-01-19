// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolActor.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/SPIRV-Headers/include/spirv/unified1/spirv.h>

#include "BullectActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "NetWorkProject1/NetWorkProject1Character.h"
#include "BullectActor.h"
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

//OnOverlap �� Ư���� Ŭ���̾�Ʈ , ���� ��ο��� ����
void APistolActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//�÷��̾��� �浹ó��
	//���Ͽ� ���̰� �ʹ� => ������ ������Ʈ�� �ִ� => attach to component ���� �ؾ��Ѵ�
	ANetWorkProject1Character* player =Cast<ANetWorkProject1Character>(OtherActor);

	// �� �ڽ�(��) �� ������ ���� ���� 
	if(player!=nullptr && player->GetOwningWeapon()==nullptr && GetOwner()==nullptr)
	{
		
		GrabPistol(player);
		//UE_LOG(LogTemp,Warning,TEXT("Pistol Collide Player"));
	}
}

void APistolActor::GrabPistol(class ANetWorkProject1Character* player)
{
	//playercontroller�� possess �����ʴ� ���ʹ� owner�� �־�� rpc�� ����Ҽ� �ִ�
	
	if(HasAuthority()) //������ ���
	{
		SetOwner(player);	//���� ���ʸ� �÷��̾�� ���� 
		ServerGrabPistol_Implementation(player);
	}
	/*else if(player->GetController() && player->GetController()->IsLocalController())
	{
		//���� ��Ʈ�ѷ��� ��쿡�� ���� rpc �Լ��� ����
		ServerGrabPistol(player);	
	}*/
		
}
void APistolActor::ServerGrabPistol_Implementation(ANetWorkProject1Character* player)
{
	player->SetOwningWeapon(this);	//����������

	player->setWeaponInfo(ammo,damagePower,attackDelay);
	
	MulticastGrabPistol(player);
}

void APistolActor::MulticastGrabPistol_Implementation(ANetWorkProject1Character* player)
{
	boxComp->SetSimulatePhysics(false);
	//AttachToComponent�� �ϱ� ���ؼ� SetSimulatePhysics �� �����Ǿ��־����  
	AttachToComponent(player->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("PistolSocket"));
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void APistolActor::ReleaseWeapon(class ANetWorkProject1Character* player)
{
	if(player==nullptr) {return;}
	// �и��ϱ� 
	if(HasAuthority())
	{
		ServerReleasePistol_Implementation(player);
	}
	else
	{
		ServerReleasePistol(player);
	}
}

void APistolActor::Fire(ANetWorkProject1Character* player)
{
	if (bullect_bp!=nullptr && GetOwner()->HasAuthority())
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ABullectActor* bullect = GetWorld()->SpawnActor<ABullectActor>(bullect_bp,meshComp->GetSocketLocation(FName
		("FirePosition")),meshComp->GetSocketRotation(FName("FirePosition")),params);
		if(bullect!=nullptr)
		{
			bullect->SetOwner(player);
			bullect->damage = damagePower;
		}
	}
	
}

void APistolActor::MulticastReleasePistol_Implementation(ANetWorkProject1Character* player)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	// �ٽ� ���� , �浹 �ۿ��Ҽ��ֵ��� �ǵ�����
	boxComp->SetSimulatePhysics(true);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//���� �÷��̾ �ٷ� �ٽ� ���������ʵ��� �÷��̾�(pawn) Ÿ�Կ� ���� �浹������ ignore���� ����
	FTimerHandle collisionHandle;
	GetWorldTimerManager().SetTimer(collisionHandle,this,&APistolActor::SetCollisionResponse,3.0f,false);
	//Ÿ�̸Ӵ� �Ű����� ���� �Ұ� ���� 
}

void APistolActor::ServerReleasePistol_Implementation(ANetWorkProject1Character* player)
{
	// �����ϱ� �÷��̾��� owningWeapon ������ null ����
	// owningWeapon ������ replicated ���־ ���������� ����ǵ� , �̰� ����Ǽ� ��ü���� ����ȭ ���ۿ��� 
	player->SetOwningWeapon(nullptr);

	ammo=player->GetAmmo();
	//���� �߿� !! �ѿ� ���� ������ �÷��̾ �̿��ؼ� ���� �����
	player->setWeaponInfo(0,0,0);
	// ���� �������ϱ� �÷��̾ ��� ������ �ʱ�ȭ

	
	MulticastReleasePistol(player); //��ġ �߿� !!
	
	SetOwner(nullptr);
	// *** �� �ڽ�(pistol)�� owner�� null���� ����� : ���������� �ϱ� +��Ŷ����� ���� �������� �ϱ� **
}


void APistolActor::SetCollisionResponse()
{
	boxComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
}

