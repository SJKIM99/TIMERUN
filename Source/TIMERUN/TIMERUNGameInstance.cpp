// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameInstance.h"

UTIMERUNGameInstance::UTIMERUNGameInstance()
{

}

void UTIMERUNGameInstance::InitLoginSocket()
{
	if (socketmgr.ConnectLoginServer()) {
		UE_LOG(LogTemp, Warning, TEXT("Success the login server connect"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fail the login server connect"));
	}
}

void UTIMERUNGameInstance::InitIngameSocket()
{
	if (socketmgr.ConnectIngameServer()) {
		UE_LOG(LogTemp, Warning, TEXT("Success the Ingame server connect"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fail the Ingame server connect"));
	}
}

//void UTIMERUNGameInstance::Init()
//{
//	Super::Init();
//
//	UE_LOG(LogTemp, Warning, TEXT("UCPP_CobotGameInstance::Init()"));
//
//	if (socketmgr.ConnectLoginServer()) {
//		UE_LOG(LogTemp, Warning, TEXT("Success the login server connect"));
//	}
//	else {
//		UE_LOG(LogTemp, Warning, TEXT("Fail the login server connect"));
//	}
//}
