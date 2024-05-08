#pragma once
#include "Session.h"
#include "WorkerThread.h"

class IngameMain
{
	WSADATA WsaData;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN client_addr;

	std::vector <std::thread> worker_threads;
	WorkerThread workerThread;
	OVER_EXP g_a_over;
public:
	//HANDLE h_iocp;
public:
	IngameMain();
	~IngameMain();
public:
	bool InitSocket();
	void ServeurRun();
};