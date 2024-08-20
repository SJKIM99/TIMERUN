#pragma once
#include "Session.h"

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
public:
    WorkerThread();
    ~WorkerThread();

private:
    concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
public:
    HANDLE h_iocp;

    std::mutex timer_lock;
    std::mutex change_lock;
public:
    void woker_thread(HANDLE h_iocp);
    void timer();

    int get_new_client_id();
    void InitPlayerInfo(int player_id);
    void ProcessPacket(int c_id, char* packet);
    int get_new_gravitybox_id();


    /*  int first_half = 60;
      int second_half = 60;
      int half_time = 5;

      int SECONDS = 60 + 60 + 5;*/

    int world_timer = 0;

    int SECONDS = 245;
};

static int game_start_player_num = 0;




