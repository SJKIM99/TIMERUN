// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameInstance.h"

UTIMERUNGameInstance::UTIMERUNGameInstance()
{

}

void UTIMERUNGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Warning, TEXT("UCPP_CobotGameInstance::Init()"));

	if (socketmgr.ConnectServer()) {
		UE_LOG(LogTemp, Warning, TEXT("Success the login server connect"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fail the login server connect"));
	}
}
