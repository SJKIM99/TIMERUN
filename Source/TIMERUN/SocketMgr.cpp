// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketMgr.h"

SocketMgr::SocketMgr()

{
}

SocketMgr::~SocketMgr()
{
}

bool SocketMgr::ConnectLoginServer()
{
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0) return false;

	login_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == login_socket) return false;

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	//server_addr.sin_addr.s_addr = inet_addr("210.99.254.132");
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_addr.s_addr = inet_addr("220.94.221.205");

	nRet = connect(login_socket, (sockaddr*)&server_addr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) return false;

	u_long nonBlockingMode = 1;
	ioctlsocket(login_socket, FIONBIO, &nonBlockingMode);
	return true;
}

bool SocketMgr::ConnectIngameServer()
{
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0) return false;

	ingame_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == ingame_socket) return false;

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);
	//server_addr.sin_addr.s_addr = inet_addr("210.99.254.132");
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_addr.s_addr = inet_addr("220.94.221.205");

	nRet = connect(ingame_socket, (sockaddr*)&server_addr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) return false;

	u_long nonBlockingMode = 1;
	ioctlsocket(ingame_socket, FIONBIO, &nonBlockingMode);

	return true;
}

