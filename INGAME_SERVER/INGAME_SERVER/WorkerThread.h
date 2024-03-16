#pragma once
#include "Session.h"

class WorkerThread
{
private:
	WSAOVERLAPPED* over = nullptr;
public:
	WorkerThread();
	~WorkerThread();

private:
	concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
public:
	void woker_thread(HANDLE h_iocp);
	//void timer_thread();
	//void world_update_thread();
	//void WorldUpdate(int c_id);
	int get_new_client_id();
	int get_new_client_in_channel_id();
	void InitPlayerInfo(int player_id);
	void ProcessPacket(int c_id, char* packet);
	int get_new_gravitybox_id();
	bool isEnterPlayer = false;
};


