// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "TIMERUNGameInstance.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "TIMERUNPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API ATIMERUNPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
private:
	SOCKET* client_socket;
	vector_d3 location;
	int id;

	virtual void Tick(float DeltaTime) override;
	void RecvPacket();
	void ProcessPakcet(char* packet);
	void SendMovePacket(direction direction);

	UTIMERUNGameInstance* instance;

	int prev_remain_data;
	int prev_packet_size;
	char prev_packet_buf[8192];
public:
	virtual void SetupInputComponent() override;
private:
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveLeft(float Value);
	void MoveBack(float Value);

	void Jump();
};
