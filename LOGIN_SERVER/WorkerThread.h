#pragma once
#include "Session.h"
#include "DBThread.h"


class WorkerThread
{
private:
	WSAOVERLAPPED* over = nullptr;
	DBThread* dbThreadInstance;
public:
	WorkerThread(){};
	~WorkerThread() {};
	WorkerThread(DBThread* dbThread);
public:
public:
	void woker_thread(HANDLE h_iocp);
	int get_new_client_id();
	void InitPlayerInfo(int player_id);
	void ProcessPakcet(int c_id, char* packet);
};

