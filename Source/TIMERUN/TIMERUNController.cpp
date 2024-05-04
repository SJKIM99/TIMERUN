// Fill out your copyright notice in the Description page of Project Settings.

#include "TIMERUNController.h"
#include "TIMERUNGameInstance.h"


ATIMERUNController::ATIMERUNController()
{
}

ATIMERUNController::~ATIMERUNController()
{
}

void ATIMERUNController::BeginPlay()
{
	Super::BeginPlay();

	instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

	instance->GetSocketMgr()->ConnectLoginServer();

	instance->login_socket = instance->GetSocketMgr()->GetLoginSocket();
}

void ATIMERUNController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!instance->IsActiveIngameSocket)
		instance->RecvPacketFromLoginServer();
	else
		instance->RecvPacketFromIngameServer();

}
