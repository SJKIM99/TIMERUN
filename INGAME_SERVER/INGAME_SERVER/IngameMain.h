#pragma once
#include "Session.h"
#include "WorkerThread.h"

class IngameMain
{
	WSADATA WsaData;
	HANDLE h_iocp;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN client_addr;

	OVER_EXP g_a_over;
private:
	std::vector <std::thread> worker_threads;
	WorkerThread workerThread;
public:
	IngameMain();
	~IngameMain();
public:
	bool InitSocket();
	void ServeurRun();
};

