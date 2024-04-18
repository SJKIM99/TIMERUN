// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "SocketMgr.h"
#include "TIMERUNGameInstance.h"
#include "TIMERUNCharacter.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"

#include <iostream>

#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "TIMERUNController_Ver2.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API ATIMERUNController_Ver2 : public APlayerController
{
	GENERATED_BODY()
	
	ATIMERUNController_Ver2();
	~ATIMERUNController_Ver2();
protected:
	virtual void BeginPlay() override;
private:
	SOCKET* login_socket;
	SOCKET* ingame_socket;
	int my_id;
	int other_id;
	TArray<AActor*> spawnedCharacters;
public:
	virtual void Tick(float DeltaTime) override;
	void RecvPacketFromLoginServer();
	void RecvPacketFromIngameServer();
	void ProcessPakcet(char* packet);
	void SendMovePacket(/*direction direction,*/ APawn* pawn);

	UTIMERUNGameInstance* instance;
	//RecvFromLoginn
	int login_prev_remain_data;
	int login_prev_packet_size;
	char login_prev_packet_buf[10000];

	//RecvFromIngame
	int ingame_prev_remain_data;
	int ingame_prev_packet_size;
	char ingame_prev_packet_buf[10000];

	bool IsActiveIngameSocket = false;
//public:
	//virtual void SetupInputComponent() override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool isPlayermove;
private:
	int nPlayers;
	bool IsEnterNewPlayer;
	Session* NewPlayer;
	void UpdateNewPlayer(int c_id);
	void SortPlayerIndex();

};
