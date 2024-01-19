// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NetworkGameModeBase.h"
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

void ANetworkPlayerController::ChangeCharToSpectator()
{
	// 1. ���� ���� possess �ϰ� �ִ� pawn ���� ���� unpossess
	APawn* possedPawn = GetPawn();
	OnUnPossess();
	
	// 2.unpossess �� pawn �� ����
	possedPawn->Destroy();
	
	// 3. gamemode �� spectatorPawn �� ����
	 ASpectatorPawn* spectatorpawn =SpawnSpectatorPawn();
	 APawn* possessPawn = Cast<APawn>(spectatorpawn);
	
	// 4. ������ spectatorPawn�� ���� possess�Ѵ�
	OnPossess(possessPawn);
}
