// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "SocketMgr.h"

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
#include "TIMERUNController.generated.h"

/**
 * 
 */
class UTIMERUNGameInstance;
UCLASS()
class TIMERUN_API ATIMERUNController : public APlayerController
{
	GENERATED_BODY()
	
	ATIMERUNController();
	~ATIMERUNController();
protected:
	virtual void BeginPlay() override;
//private:
//	int my_id;
//	int other_id;
public:
	virtual void Tick(float DeltaTime) override;
	UTIMERUNGameInstance* instance;
};
