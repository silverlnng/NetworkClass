// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

// 레벨이 변경 => 모두 새롭게 재입장 !! (세션에 한번입장 개념이 아님 ) 
// 함수호출 => 플레이어 입장할때 마다 
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
