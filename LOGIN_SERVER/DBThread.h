#pragma once
#include "DataBase.h"

enum DB_EVENT_TYPE { EV_SAVE_INFO };

struct DB_EVENT {
    int player_id;
    std::chrono::system_clock::time_point wakeup_time;
    TIMER_EVENT_TYPE event;
    constexpr bool operator < (const DB_EVENT& L) const
    {
        return (wakeup_time > L.wakeup_time);
    }
};

class DBThread
{
    DataBase* m_database;
    concurrency::concurrent_priority_queue<DB_EVENT> m_timer_queue;

public:
    DBThread();
    ~DBThread();
public:
    void db_thread();
    DataBase* GetDatabase() { return m_database; }
};
