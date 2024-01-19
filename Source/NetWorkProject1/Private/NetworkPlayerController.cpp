// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NetworkGameModeBase.h"
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

void ANetworkPlayerController::ChangeCharToSpectator()
{
	// 1. 현재 내가 possess 하고 있는 pawn 으로 부터 unpossess
	APawn* possedPawn = GetPawn();
	OnUnPossess();
	
	// 2.unpossess 된 pawn 을 제거
	possedPawn->Destroy();
	
	// 3. gamemode 의 spectatorPawn 을 생성
	 ASpectatorPawn* spectatorpawn =SpawnSpectatorPawn();
	 APawn* possessPawn = Cast<APawn>(spectatorpawn);
	
	// 4. 생성된 spectatorPawn을 소유 possess한다
	OnPossess(possessPawn);
}
