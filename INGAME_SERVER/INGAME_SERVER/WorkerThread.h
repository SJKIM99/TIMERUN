#pragma once
#include "Session.h"

class WorkerThread
{
private:
	WSAOVERLAPPED* over = nullptr;
public:
	WorkerThread() {};
	~WorkerThread() {};
public:
	void woker_thread(HANDLE h_iocp);
	int get_new_client_id();
	int get_new_client_in_channel_id();
	void InitPlayerInfo(int player_id);
	void ProcessPacket(int c_id, char* packet);
};

