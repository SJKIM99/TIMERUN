// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWidget.h"
#include "TIMERUNGameInstance.h"

void UBeginWidget::NativeOnInitialized()
{
	instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());
}

bool UBeginWidget::SendLoginPacket()
{
	u_long blockingmode = 0;
	ioctlsocket(*instance->login_socket, FIONBIO, &blockingmode);

	CS_LOGIN_PACKET packet;
	packet.size = sizeof CS_LOGIN_PACKET;
	packet.type = CS_LOGIN;
	strcpy_s(packet.id, TCHAR_TO_ANSI(*ID));
	strcpy_s(packet.passwd, TCHAR_TO_ANSI(*PASSWD));

	send(*instance->login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

	char recv_buf[BUFSIZ];

	recv(*instance->login_socket, reinterpret_cast<char*>(recv_buf), BUFSIZ, 0);

	u_long nonblockingmode = 1;
	ioctlsocket(*instance->login_socket, FIONBIO, &nonblockingmode);

	switch (recv_buf[1]) {
	case SC_LOGIN_SUCCESS: {
		SC_LOGIN_SUCCESS_PACKET* p = reinterpret_cast<SC_LOGIN_SUCCESS_PACKET*>(recv_buf);
		NICKNAME = p->nickname;
		UE_LOG(LogTemp, Warning, TEXT("NICKNAME : "),*NICKNAME);
		return true;
	}
	case SC_LOGIN_FAIL: {
		return false;
	}
	}
	return false;
}

bool UBeginWidget::SendSignUpPacket()
{
	u_long blockingmode = 0;
	ioctlsocket(*instance->login_socket, FIONBIO, &blockingmode);

	CS_SIGNUP_PACKET packet;
	packet.size = sizeof CS_SIGNUP_PACKET;
	packet.type = CS_SIGNUP;
	strcpy_s(packet.id, TCHAR_TO_ANSI(*ID));
	strcpy_s(packet.passwd, TCHAR_TO_ANSI(*PASSWD));
	strcpy_s(packet.nickname, TCHAR_TO_ANSI(*NICKNAME));

	send(*instance->login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

	char recv_buf[BUFSIZ];

	recv(*instance->login_socket, reinterpret_cast<char*>(recv_buf), BUFSIZ, 0);
	switch (recv_buf[1]) {
	case SC_SIGNUP_SUCCESS: {
		return true;
	}
	case SC_LOGIN_FAIL: {
		return false;
	}
	}
	return false;
}


