#include "pch.h"
#include "DBThread.h"

DBThread::DBThread()
{
    m_database = new DataBase;
    if (!m_database->DBConnect()) {
        std::cout << "DB Connect failed" << std::endl;
    }
    else {
        std::cout << "DB Connect Success" << std::endl;
    }
}

DBThread::~DBThread()
{
    delete m_database;
}

void DBThread::db_thread()
{
    while (true) {
        DB_EVENT ev;
        auto current_time = std::chrono::system_clock::now();
        if (true == m_timer_queue.try_pop(ev)) {
            m_timer_queue.push(ev);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        switch (ev.event) {
        case EV_SAVE_INFO: {

        }
                         break;
        }
    }
}