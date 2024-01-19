// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NetworkGameModeBase.h"
#include "GameFramework/SpectatorPawn.h"

void ANetworkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bReplicates=true;

	
	if(HasAuthority()) //주의사항 GameMode는 서버만 있음 
	{
	gm = GetWorld()->GetAuthGameMode<ANetworkGameModeBase>();
	}

	//어차피 게임모드는 서버버전이어서 rpelicated할필요 없음..
	
}



void ANetworkPlayerController::ChangeCharToSpectator_Implementation()
{
	// 1. 현재 내가 possess 하고 있는 pawn 으로 부터 unpossess
	APawn* prevPawn = GetPawn();
	//OnUnPossess();
	UnPossess();
	
	// 3. gamemode 의 spectatorPawn 을 생성
	 ASpectatorPawn* spectatorpawn =SpawnSpectatorPawn();
	 APawn* possessPawn = Cast<APawn>(spectatorpawn);

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	 ASpectatorPawn* spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass,prevPawn->GetActorTransform(), 
	 params);
	
	// 4. 생성된 spectatorPawn을 소유 possess한다
	//OnPossess(possessPawn);
	
	Possess(possessPawn);
	
	// 2.unpossess 된 pawn 을 제거 =>파괴를 제일 마지막에 하자 !
	prevPawn->Destroy();
	
	//쿨타임을 주고 respawn 되도록 만들기
	float coolTime = 5.f;
	FTimerHandle respawnHandler;
	GetWorldTimerManager().SetTimer(respawnHandler,this,&ANetworkPlayerController::serverRespawnPlayer,coolTime,false);
}

void ANetworkPlayerController::serverRespawnPlayer_Implementation()
{
	APawn* specPawn = GetPawn();
	UnPossess();
	//gm->RestartPlayer(this);	//최초 시작위치에서 부활
	FTransform restartsTrans;
	restartsTrans.SetLocation(FVector(2200,700,220));
	
	gm->RestartPlayerAtTransform(this,restartsTrans);
	//특정위치에서 부활
	specPawn->Destroy();
}