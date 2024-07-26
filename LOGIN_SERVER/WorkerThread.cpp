#include "pch.h"
#include "WorkerThread.h"

OVER_EXP g_over;

WorkerThread::WorkerThread(DBThread* dbThread) : dbThreadInstance(dbThread) {}

void WorkerThread::woker_thread(HANDLE h_iocp)
{
    while (true) {
        DWORD num_bytes;
        ULONG_PTR key;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
        OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
        if (FALSE == ret) {
            if (ex_over->comp_type == OP_ACCEPT) std::cout << "Accept Error";
            else {
                std::cout << "GQCS Error on client[" << key << "]\n";
                disconnect(static_cast<int>(key));
                if (ex_over->comp_type == OP_SEND) delete ex_over;
                continue;
            }
        }

        if ((0 == num_bytes) && ((ex_over->comp_type == OP_RECV) || (ex_over->comp_type == OP_SEND))) {
            disconnect(static_cast<int>(key));
            if (ex_over->comp_type == OP_SEND) delete ex_over;
            continue;
        }

        switch (ex_over->comp_type) {
        case OP_ACCEPT: {
            int client_id = get_new_client_id();
            std::cout << client_id << "번 클라이언트가 입장하였습니다." << std::endl;
            if (client_id != -1) {
                {
                    std::lock_guard<std::mutex> ll{ clients[client_id].m_state_lock };
                    clients[client_id].m_state = ST_ALLOC;
                    //현재 로그인서버에 들어온 플레이어 수
                    ++enter_player_num;
                }
                InitPlayerInfo(client_id);
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_client_socket), h_iocp, client_id, 0);
                clients[client_id].RecvPacket();
                g_client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            }
            else
                std::cout << "Max user exceeded.\n";
            //ZeroMemory(&g_over.over, sizeof(g_over.over));

            g_over.comp_type = OP_ACCEPT;
            g_over.wsabuf.len = BUF_SIZE;
            int addr_size = sizeof(SOCKADDR_IN);
            AcceptEx(g_server_socket, g_client_socket, g_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_over.over);
        }
                      break;
        case OP_RECV: {
            int remain_data = num_bytes + clients[key].m_prev_remain_data;
            char* p = ex_over->send_buf;
            while (remain_data > 0) {
                int packet_size = p[0];
                if (packet_size <= remain_data) {
                    ProcessPacket(static_cast<int>(key), p);
                    p = p + packet_size;
                    remain_data = remain_data - packet_size;
                }
                else break;
            }
            clients[key].m_prev_remain_data = remain_data;
            if (remain_data > 0) {
                memcpy(ex_over->send_buf, p, remain_data);
            }
            clients[key].RecvPacket();
        }
                    break;
        case OP_SEND: {
            delete ex_over;
        }
                    break;
        case OP_GAME_START: {
            std::cout << "Game Start" << std::endl;
            clients[key].m_ready_lock.lock();
            for (auto& cl : clients) {
                if (ST_FREE == cl.m_state) break;
                cl.m_ready = false;
                cl.send_game_start_packet(cl.m_id);
            }
            clients[key].m_ready_lock.unlock();
            delete ex_over;
        }
                          break;
        }
    }
}

void WorkerThread::timer()
{
    while (true) {
        TIMER_EVENT timer_event;
        auto current_time = std::chrono::system_clock::now();
        if (true == timer_queue.try_pop(timer_event)) {
            if (timer_event.wakeup_time > current_time) {
                timer_queue.push(timer_event);
                continue;
            }
            switch (timer_event.event) {
            case EV_GAME_START: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_GAME_START;
                player_ready_num = 0;
                PostQueuedCompletionStatus(h_iocp, 1, timer_event.object_id, &ov->over);
            }
                              break;
            }
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int WorkerThread::get_new_client_id()
{
    for (int i = 0; i < MAX_USER; ++i) {
        std::lock_guard <std::mutex> ll{ clients[i].m_state_lock };
        if (clients[i].m_state == ST_FREE) {
            clients[i].m_state = ST_ALLOC;
            return i;
        }
    }
    return -1;
}

void WorkerThread::InitPlayerInfo(int player_id)
{
    clients[player_id].m_socket = g_client_socket;
    clients[player_id].m_id = player_id;
    clients[player_id].m_name[0] = 0;
    clients[player_id].m_prev_remain_data = 0;
    clients[player_id].m_online = true;
}

void WorkerThread::ProcessPacket(int c_id, char* packet)
{
    switch (packet[1]) {
    case CS_LOGIN: {
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
        if (dbThreadInstance->GetDatabase()->IsIDRegistered(p->id) && dbThreadInstance->GetDatabase()->IsPASSWDRegistered(p->passwd)) {
            //여기서 클라이언트에게 다시 session정보와 인게임서버토큰을 전송한다
            clients[c_id].send_login_success_packet(dbThreadInstance->GetDatabase()->ExtractPlayerInfo(p->id, p->passwd));
            strcpy_s(clients[c_id].m_name, dbThreadInstance->GetDatabase()->ExtractPlayerInfo(p->id, p->passwd).player_nickname);
        }
        else {
            clients[c_id].send_login_fail_packet();
        }
    }
                 break;
    case CS_SIGNUP: {
        CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);
        if (dbThreadInstance->GetDatabase()->RegisteredAccount(c_id, p->id, p->passwd, p->nickname)) {
            clients[c_id].send_signup_success_packet();
        }
        else {
            clients[c_id].send_signup_fail_packet();
        }
    }
                  break;
    case CS_READY: {
        CS_READY_PACKET* p = reinterpret_cast<CS_READY_PACKET*>(packet);
        {
            std::cout << c_id << "번 클라 레디패킷 전송" << std::endl;
            std::lock_guard<std::mutex> readylock(clients[c_id].m_ready_lock);

            if (false == clients[c_id].m_ready) {
                clients[c_id].m_ready = true;
                ++player_ready_num;
            }
            else { 
                clients[c_id].m_ready = false;
                --player_ready_num;
            }

            /*for (auto& cl : clients) {
                if (cl.m_state == ST_FREE) break;
                cl.send_ready_packet(c_id);
            }*/
            clients[c_id].send_ready_packet(c_id);
            std::cout << "레디한 플레이어 수 : " << player_ready_num << std::endl;
            if (player_ready_num % 2 == 0 && player_ready_num !=0) {
                TIMER_EVENT event{ enter_player_num,std::chrono::system_clock::now() + std::chrono::seconds(GMAE_START_COOLTIME),EV_GAME_START,0 };
                for (auto& cl : clients) {
                    if (cl.m_state == ST_FREE) break;
                    cl.send_all_player_ready_packet();
                }
                timer_queue.push(event);
            }

        }

       
    }
                 break;
    }
}