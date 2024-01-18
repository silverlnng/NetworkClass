// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

//아무조건이 없다면 함수호출되는순서 => 들어오는 순서
// AController* player  을 인자로 받아서 플레이어에 접근을 해서 플레이어의 기준으로 설정도 가능하다 !
AActor* ANetworkGameModeBase::ChoosePlayerStart_Implementation(AController* player)
{
	TArray<APlayerStart*> playerStartActors;
	
	playerStartActors.SetNum(2); 
	
	for(TActorIterator<APlayerStart> playerStart(GetWorld()); playerStart; ++playerStart)
	{
		APlayerStart* ps = *playerStart;
		
		if(ps->ActorHasTag(FName("Red")))
		{
			playerStartActors[0] = ps;
		}
		else
		{
			playerStartActors[1] = ps;	
		}
		//playerStartActors.Add(*playerStart);
	}
	int32 num = index++ % playerStartActors.Num();
	return playerStartActors[num];
}
