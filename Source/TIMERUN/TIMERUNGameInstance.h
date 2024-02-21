// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
public:
	SocketMgr* GetSocketMgr() {
		return &socketmgr;
	}
	/*virtual void Init() override;*/
	void InitLoginSocket();
	void InitIngameSocket();
};
