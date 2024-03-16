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
#include "GameFramework/Controller.h"
#include "GravityBoxController.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API AGravityBoxController : public AController
{
	GENERATED_BODY()
public:
	AGravityBox* gravitybox;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
