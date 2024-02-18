// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

/**
 * 
 */
class TIMERUN_API SocketMgr
{
public:
	SocketMgr();
	~SocketMgr();
private:
	SOCKET client_socket;
public:
	SOCKET* GetSocket() {
		return &client_socket;
	}
public:
	bool ConnectServer();
};
