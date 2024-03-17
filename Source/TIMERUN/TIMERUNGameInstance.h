// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "TIMERUNCharacter.h"
#include "GravityBox.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"

#include "SocketMgr.h"
#include "Engine/GameInstance.h"
#include "TIMERUNGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API UTIMERUNGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UTIMERUNGameInstance();
private:
	SocketMgr socketmgr;
	SOCKET* login_socket;
	SOCKET* ingame_socket;

	int my_id;
	int other_id;
public:
	void RecvPacketFromLoginServer();
	void RecvPacketFromIngameServer();
	void ProcessPakcet(char* packet);
	void SendPlayerupdatePakcet();

	//RecvFromLoginn
	int login_prev_remain_data;
	int login_prev_packet_size;
	char login_prev_packet_buf[10000];

	//RecvFromIngame
	int ingame_prev_remain_data;
	int ingame_prev_packet_size;
	char ingame_prev_packet_buf[10000];

	bool IsActiveIngameSocket = false;
private:
	int nPlayers;
	bool IsEnterNewPlayer;
	void UpdateNewPlayer(int c_id);
	void SortPlayerIndex();

public:
	TArray<AActor*> spawnedCharacters;	//플레이어 동기화 배열 
	FTimerHandle SendPlayerInfoHandle;	// 동기화 타이머 핸들러
	TArray<AActor*> spawnedGravityBox;
	void UpdateNewGravityBox(FVector location, FRotator rotation, int box_id);
public:
	SocketMgr* GetSocketMgr() {
		return &socketmgr;
	}
	void InitLoginSocket();
	void InitIngameSocket();
};
