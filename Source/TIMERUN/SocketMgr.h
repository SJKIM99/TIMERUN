// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "CoreMinimal.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <iostream>
#include <array>

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

/**
 * 
 */
struct Session {
	int id;
	char NickName[NAMESIZE];

	vector_d3 location;
	float Yaw;
};

extern std::array<Session, MAX_CLIENTS> players;

class TIMERUN_API SocketMgr
{
public:
	SocketMgr();
	~SocketMgr();
private:
	SOCKET login_socket;
	SOCKET ingame_socket;
public:
	SOCKET* GetLoginSocket() {
		return &login_socket;
	}
	SOCKET* GetIngameSocket() {
		return &ingame_socket;
	}
public:
	bool ConnectLoginServer();
	bool ConnectIngameServer();
};


