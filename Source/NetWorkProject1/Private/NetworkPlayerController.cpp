// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NetworkGameModeBase.h"
#include "GameFramework/SpectatorPawn.h"

void ANetworkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bReplicates=true;

	
	if(HasAuthority()) //���ǻ��� GameMode�� ������ ���� 
	{
	gm = GetWorld()->GetAuthGameMode<ANetworkGameModeBase>();
	}

	//������ ���Ӹ��� ���������̾ rpelicated���ʿ� ����..
	
}



void ANetworkPlayerController::ChangeCharToSpectator_Implementation()
{
	// 1. ���� ���� possess �ϰ� �ִ� pawn ���� ���� unpossess
	APawn* prevPawn = GetPawn();
	//OnUnPossess();
	UnPossess();
	
	// 3. gamemode �� spectatorPawn �� ����
	 ASpectatorPawn* spectatorpawn =SpawnSpectatorPawn();
	 APawn* possessPawn = Cast<APawn>(spectatorpawn);

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	 ASpectatorPawn* spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass,prevPawn->GetActorTransform(), 
	 params);
	
	// 4. ������ spectatorPawn�� ���� possess�Ѵ�
	//OnPossess(possessPawn);
	
	Possess(possessPawn);
	
	// 2.unpossess �� pawn �� ���� =>�ı��� ���� �������� ���� !
	prevPawn->Destroy();
	
	//��Ÿ���� �ְ� respawn �ǵ��� �����
	float coolTime = 5.f;
	FTimerHandle respawnHandler;
	GetWorldTimerManager().SetTimer(respawnHandler,this,&ANetworkPlayerController::serverRespawnPlayer,coolTime,false);
}

void ANetworkPlayerController::serverRespawnPlayer_Implementation()
{
	APawn* specPawn = GetPawn();
	UnPossess();
	//gm->RestartPlayer(this);	//���� ������ġ���� ��Ȱ
	FTransform restartsTrans;
	restartsTrans.SetLocation(FVector(2200,700,220));
	
	gm->RestartPlayerAtTransform(this,restartsTrans);
	//Ư����ġ���� ��Ȱ
	specPawn->Destroy();
}