// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameStateBase.h"
#include "GameFramework/PlayerState.h"

TArray<APlayerState*> ANetGameStateBase::GetMyPlayerList()
{
	//PlayerArray.Sort(ANetGameStateBase::AscendingByString);
	// Sort() :(������) �ȿ� ���ǽ��� �־

	// 1) static ���ǽ� �Լ��� �����ϴ� ��� 
	PlayerArray.Sort(ANetGameStateBase::AscendingByString);

	// 2) ���ٽ����� ����ϴ� ��� 
	PlayerArray.Sort([](const APlayerState& ps1, const APlayerState& ps2)
	{
		return ps1.GetPlayerName() < ps2.GetPlayerName();
	});

	//������ ����
	PlayerArray.Sort([](const APlayerState& ps1, const APlayerState& ps2)
	{
		return ps1.GetScore() >= ps2.GetScore();
	});
	
	return PlayerArray;
}

bool ANetGameStateBase::AscendingByString(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetPlayerName() < ps2.GetPlayerName();

	//ps1->GetPlayerName() < ps2->GetPlayerName(); �ƽ�Ű�ڵ带 �� :  �������� ���� => ������ ���� ��ũ�� true�� ��ȯ
}

bool ANetGameStateBase::DescendingByString(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetPlayerName() > ps2.GetPlayerName();
}

bool ANetGameStateBase::DescendingByScore(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetScore() >= ps2.GetScore();
}

bool ANetGameStateBase::AscendingByScore(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetScore() < ps2.GetScore();
}
