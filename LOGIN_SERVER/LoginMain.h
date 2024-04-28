#pragma once
#include "Session.h"
#include "WorkerThread.h"
#include "DBThread.h"

class LoginMain
{
	WSADATA WsaData;
	//HANDLE h_iocp;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN client_addr;

	OVER_EXP g_a_over;
private:
	std::vector <std::thread> worker_threads;
	DBThread* dbThread; 
	WorkerThread workerThread; 
public:
	LoginMain();
	~LoginMain();
public:
	bool InitSocket();
	void ServerRun();
};

