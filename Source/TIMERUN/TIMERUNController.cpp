// Fill out your copyright notice in the Description page of Project Settings.

#include "TIMERUNGameInstance.h"
#include "TIMERUNController.h"


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

	login_socket = instance->GetSocketMgr()->GetLoginSocket();

	CS_LOGIN_PACKET packet;
	packet.size = sizeof CS_LOGIN_PACKET;
	packet.type = CS_LOGIN;
	strcpy_s(packet.id, "sungjun426");
	strcpy_s(packet.passwd, "wkd5306");

	int ret = send(*login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void ATIMERUNController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (instance->IsEnterNewPlayer)
		instance->RecvPacketFromLoginServer();
	else
		instance->RecvPacketFromIngameServer();
	if(instance->IsEnterNewPlayer)
		instance->UpdateNewPlayer(other_id);

	/*if (!IsActiveIngameSocket)
		RecvPacketFromLoginServer();
	else
		RecvPacketFromIngameServer();

	if (IsEnterNewPlayer)
		UpdateNewPlayer(other_id);*/
}
