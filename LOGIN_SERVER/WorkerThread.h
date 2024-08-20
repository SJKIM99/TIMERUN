#pragma once
#include "Session.h"
#include "DBThread.h"


struct TIMER_EVENT {
    int object_id;
    std::chrono::system_clock::time_point wakeup_time;
    TIMER_EVENT_TYPE event;
    int target_id;
    constexpr bool operator < (const TIMER_EVENT& L) const
    {
        return (wakeup_time > L.wakeup_time);
    }
};

class WorkerThread
{
private:
    WSAOVERLAPPED* over = nullptr;
    DBThread* dbThreadInstance;

    concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
public:
    HANDLE h_iocp;
public:
    WorkerThread() {};
    ~WorkerThread() {};
    WorkerThread(DBThread* dbThread);
public:
    void woker_thread(HANDLE h_iocp);
    void timer();

    int get_new_client_id();
    void InitPlayerInfo(int player_id);
    void ProcessPacket(int c_id, char* packet);

};

static int player_ready_num = 0;
static int enter_player_num = 0;