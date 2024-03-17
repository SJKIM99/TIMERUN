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

	CS_LOGIN_PACKET packet;
	packet.size = sizeof CS_LOGIN_PACKET;
	packet.type = CS_LOGIN;
	strcpy_s(packet.id, "sungjun426");
	strcpy_s(packet.passwd, "wkd5306");

	int ret = send(*instance->login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void ATIMERUNController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (instance->IsActiveIngameSocket)
		instance->RecvPacketFromLoginServer();
	else
		instance->RecvPacketFromIngameServer();

	if(instance->IsEnterNewPlayer)
		instance->UpdateNewPlayer(other_id);
}
