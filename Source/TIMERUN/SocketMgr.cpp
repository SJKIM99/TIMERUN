// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketMgr.h"

SocketMgr::SocketMgr()
{
}

SocketMgr::~SocketMgr()
{
}

bool SocketMgr::ConnectServer()
{
	UE_LOG(LogTemp, Warning, TEXT("SocketMgr - ConnectServer()"));

	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0) return false;

	client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == client_socket) return false;

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	nRet = connect(client_socket, (sockaddr*)&server_addr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) return false;

	u_long nonBlockingMode = 1;
	ioctlsocket(client_socket, FIONBIO, &nonBlockingMode);

	UE_LOG(LogTemp, Warning, TEXT("Success login server connect"));
	return true;
}
