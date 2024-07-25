// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "TIMERUNCharacter.h"
#include "GravityBox.h"
#include "TIMERUNController.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"

#include <chrono>
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
public:
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

  //  bool IsActiveIngameSocket = false;
public:
    int nPlayers;
    bool IsEnterNewPlayer;
    void UpdateNewPlayer(int c_id, FVector location, FString nickname, int time, bool role);
    void SortPlayerIndex();

    UFUNCTION(BlueprintCallable)
    void SendGravityBoxSpawn(FVector location, FRotator rotation);

    int nGravityBox;

public:
    TArray<AActor*> spawnedCharacters;   //플레이어 동기화 배열 
    FTimerHandle SendPlayerInfoHandle;   // 동기화 타이머 핸들러
    AGravityBox* gravitybox;
    TArray<AActor*> spawnedGravityBox;
    void UpdateNewGravityBox(FVector location, FRotator rotation, int box_id, int bywhoid, int box_time);

public:
    //보간 코드 여기에 추가
    void UpdatePosition(FVector new_location, FRotator new_rotation, FVector new_velocity, int player_id);
    void InterpolatePosition(ATIMERUNCharacter* UpdatePlayer);
    FTimerHandle MoveTimerHandle;   // 동기화 타이머 핸들러

    void UpdateGravityBoxPosition(FVector new_location, FRotator new_rotation, FVector new_velocity, int box_id);
    void InterporlateGravityBoxPosition(AGravityBox* UpdateGravityBox);
public:
    SocketMgr* GetSocketMgr() {
        return &socketmgr;
    }
    void InitLoginSocket();
    void InitIngameSocket();
public:
    UFUNCTION(BlueprintCallable)
    void SendPlayerJumpPacket();
    UFUNCTION(BlueprintCallable)
    void SendPlayerLandedPacket();
public:
    UFUNCTION(BlueprintCallable)
    void SendTimeChangePacket();
public:
    /*UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int TIMESIZE;*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool GameStart = false;
public:
    //카메라 동기화
    UFUNCTION(BlueprintCallable)
    void SendCameraScorePacket();
public:
    //월드시간 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int seconds;
public:
    FTimerHandle GameEndHandle;
    void GameEnd();
};