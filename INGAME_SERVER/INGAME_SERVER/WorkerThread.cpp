#include "pch.h"
#include "WorkerThread.h"
#include "IngameMain.h"

OVER_EXP g_over;

WorkerThread::WorkerThread()
{
}

WorkerThread::~WorkerThread()
{
}

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
            if (client_id != -1) {
                {
                    std::lock_guard<std::mutex> ll{ clients[client_id].m_state_lock };
                    clients[client_id].m_state = S_STATE::ST_ALLOC;
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
        case OP_GAME_TIMER_ON: {
            delete ex_over;

            for (auto& cl : clients) {
                if (cl.m_state == ST_FREE) break;
                cl.send_game_time_packet(SECONDS);
            }

            if (world_timer == PLAYTIME) {
                for (auto& cl : clients) {
                    if (cl.m_state == ST_FREE) break;
                    cl.send_game_end_packet();
                    TIMER_EVENT event{ cl.m_id,std::chrono::system_clock::now() + std::chrono::seconds(1),EV_GAME_END,0 };
                    timer_queue.push(event);
                }

                break;
            }
            timer_lock.lock();
            ++world_timer;
            --SECONDS;
            if (world_timer == first_half + half_time + 1) {
                for (auto& cl : clients) {
                    if (cl.m_state == ST_FREE) break;
                    cl.m_ture_chaser_false_runner = !cl.m_ture_chaser_false_runner;
                }
                timer_lock.unlock();
                for (auto& cl : clients) {
                    if (cl.m_state == ST_FREE) break;
                    cl.send_team_change_packet(cl.m_id);
                }
                std::cout << "공수교대" << std::endl;
            }
            else timer_lock.unlock();

            TIMER_EVENT event{ key,std::chrono::system_clock::now() + std::chrono::seconds(1),EV_GAME_TIMER_ON,0 };
            timer_queue.push(event);
        }
                             break;
        case OP_CAN_TAKE_PICTURE: {
            delete ex_over;
            clients[key].send_can_take_picture_packet(key);
        }
                                break;
        case OP_CAN_SPAWN_GRAVITYBOX: {
            delete ex_over;
            clients[key].send_can_spawn_gravitybox(key);
        }
                                    break;
        case OP_TIME_CHANGE: {
            delete ex_over;
            clients[key].send_can_time_change_packet(key);
        }
                           break;
        case OP_GAME_END: {
            delete ex_over;
            closesocket(clients[key].m_socket);
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
            case EV_GAME_TIMER_ON: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_GAME_TIMER_ON;
                PostQueuedCompletionStatus(h_iocp, 1, timer_event.object_id, &ov->over);
            }
                                 break;
            case EV_TEAM_CHANGE: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_TEAM_CHANGE;
                PostQueuedCompletionStatus(h_iocp, 1, timer_event.object_id, &ov->over);
            }
                               break;
            case EV_CAN_TAKE_PICTURE: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_CAN_TAKE_PICTURE;
                clients[timer_event.object_id].m_cantakepicture = true;
                PostQueuedCompletionStatus(h_iocp, 1, timer_event.object_id, &ov->over);
            }
                                    break;
            case EV_CAN_SPAWN_GRAVITYBOX: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_CAN_SPAWN_GRAVITYBOX;
                clients[timer_event.object_id].m_canspawngravitybox = true;
                PostQueuedCompletionStatus(h_iocp, 1, timer_event.object_id, &ov->over);
            }
                                        break;
            case EV_TIME_CHANGE: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_TIME_CHANGE;
                clients[timer_event.object_id].can_time_change = true;
                PostQueuedCompletionStatus(h_iocp, 1, timer_event.object_id, &ov->over);
            }
                               break;
            case EV_GAME_END: {
                OVER_EXP* ov = new OVER_EXP;
                ov->comp_type = OP_GAME_END;
                clients[timer_event.object_id].reset_player_info(timer_event.object_id);
                reset_gravitybox_info();
                SECONDS = first_half + second_half + half_time;
                world_timer = 0;
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
        if (clients[i].m_state == S_STATE::ST_FREE)
            return i;
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
    clients[player_id].m_location.x = 0;
    clients[player_id].m_location.y = 0;
    clients[player_id].m_location.z = 400;
    clients[player_id].m_yaw = 0;
    clients[player_id].m_velocity.x = 0;
    clients[player_id].m_velocity.y = 0;
    clients[player_id].m_velocity.z = 0;
    clients[player_id].m_HaveGrabityGun = false;
    clients[player_id].m_score = 0;
}

void WorkerThread::ProcessPacket(int c_id, char* packet)
{
    switch (packet[1]) {
    case CS_INGAME_LOGIN: {
        CS_INGAME_LOGIN_PACKET* p = reinterpret_cast<CS_INGAME_LOGIN_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_enter_lock);
            strcpy_s(clients[c_id].m_name, p->nickname);
            clients[c_id].m_location.x = p->location.x;
            clients[c_id].m_location.y = p->location.y;
            clients[c_id].m_location.z = p->location.z;

            if (game_start_player_num % 2 == 0) {
                clients[c_id].m_ture_chaser_false_runner = rand() % 2;
            }
            else {
                clients[c_id].m_ture_chaser_false_runner = !clients[c_id - 1].m_ture_chaser_false_runner;
            }
            clients[c_id].m_time = p->my_time;

            ++game_start_player_num;

            std::cout << c_id << "번 클라 역할 : " << clients[c_id].m_ture_chaser_false_runner << std::endl;
        }
        clients[c_id].send_ingame_login_sucess_packet(c_id);

        //다른 클라이언트한테 내가 로그인했다고 보내기
        for (auto pl : clients) {
            if (pl.m_state != ST_ALLOC) break;
            if (pl.m_id == c_id) continue;
            pl.send_add_player_packet(c_id);
        }
        //c_id클라에게 다른 플레이어 정보 보내기
        for (auto pl : clients) {
            if (pl.m_state != ST_ALLOC) break;
            if (pl.m_id == c_id) continue;
            clients[c_id].send_add_player_packet(pl.m_id);
        }
        //여기서 일단 타이머이벤트를 통해 클라이언트들에게 월드시간을 보내도록 하자

        if (game_start_player_num % 2 == 0) {

            TIMER_EVENT event{ c_id,std::chrono::system_clock::now() + std::chrono::milliseconds(1),EV_GAME_TIMER_ON,0 };
            timer_queue.push(event);
        }
    }
                        break;
    case CS_PLAYER_UPDATE: {
        CS_PLAYER_UPDATE_PACKET* p = reinterpret_cast<CS_PLAYER_UPDATE_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_container_lock);
            clients[p->id].m_location.x = p->location.x;
            clients[p->id].m_location.y = p->location.y;
            clients[p->id].m_location.z = p->location.z;

            clients[p->id].m_velocity.x = p->velocity.x;
            clients[p->id].m_velocity.y = p->velocity.y;
            clients[p->id].m_velocity.z = p->velocity.z;

            clients[p->id].m_yaw = p->yaw;
            clients[p->id].m_HaveGrabityGun = p->HaveGravityGun;

            clients[p->id].m_time = p->time;

            clients[p->id].m_isjump = p->isLanded;

            clients[p->id].m_HaveTimeMachine = p->HaveTimeMachine;

            clients[p->id].m_doingtimetravel = p->DoingTimeTravel;
        }

        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == p->id) continue;
            cl.send_world_update_packet(c_id);
        }
    }
                         break;
    case CS_GRAVITYBOX_ADD: {
        CS_GRAVITYBOX_ADD_PACKET* p = reinterpret_cast<CS_GRAVITYBOX_ADD_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_gravitybox_lock);
            int BoxId = get_new_gravitybox_id();
            gravitybox[BoxId].location.x = p->location.x;
            gravitybox[BoxId].location.y = p->location.y;
            gravitybox[BoxId].location.z = p->location.z;

            gravitybox[BoxId].rotation.x = p->rotation.x;
            gravitybox[BoxId].rotation.y = p->rotation.y;
            gravitybox[BoxId].rotation.z = p->rotation.z;

            gravitybox[BoxId].time = p->time;

            clients[c_id].m_canspawngravitybox = false;

            for (auto& cl : clients) {
                if (cl.m_state == ST_FREE) break;
                cl.send_gravitybox_add_packet(c_id, BoxId);
            }
        }
        TIMER_EVENT event{ c_id,std::chrono::system_clock::now() + std::chrono::seconds(SPAWN_GRAVITYBOX_COOLTIME),EV_CAN_SPAWN_GRAVITYBOX,0 };
        timer_queue.push(event);
    }
                          break;
    case CS_GRAVITYBOX_UPDATE: {
        CS_GRAVITYBOX_UPDATE_PACKET* p = reinterpret_cast<CS_GRAVITYBOX_UPDATE_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_gravitybox_lock);

            gravitybox[p->boxid].ByWhoId = p->bywhoid;
            gravitybox[p->boxid].isGrabbed = p->isgrabbed;
            gravitybox[p->boxid].location.x = p->location.x;
            gravitybox[p->boxid].location.y = p->location.y;
            gravitybox[p->boxid].location.z = p->location.z;

            gravitybox[p->boxid].rotation.x = p->rotation.x;
            gravitybox[p->boxid].rotation.y = p->rotation.y;
            gravitybox[p->boxid].rotation.z = p->rotation.z;

            gravitybox[p->boxid].velocity.x = p->velocity.x;
            gravitybox[p->boxid].velocity.y = p->velocity.y;
            gravitybox[p->boxid].velocity.z = p->velocity.z;

            gravitybox[p->boxid].time = p->time;
            gravitybox[p->boxid].grabbed_time = p->grabbed_time;

            gravitybox[p->boxid].IsMoving = p->ismoving;
            gravitybox[p->boxid].CanFixPos = p->canfixpos;
            gravitybox[p->boxid].CanFall = p->canfall;
        }
        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == c_id) continue;
            cl.send_gravitybox_update_packet(c_id, p->boxid);
        }
    }
                             break;
    case CS_PLAYER_JUMP: {
        CS_PLAYER_JUMP_PACKET* p = reinterpret_cast<CS_PLAYER_JUMP_PACKET*>(packet);

        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == c_id) continue;
            cl.send_player_jump_packet(c_id);
        }
    }
                       break;
    case CS_GRAVITYBOX_GRABBED: {
        CS_GRAVITYBOX_GRABBED_PACKET* p = reinterpret_cast<CS_GRAVITYBOX_GRABBED_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_gravitybox_lock);
            gravitybox[p->boxid].isGrabbed = p->isGrabbed;

        }
        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == c_id) continue;
            cl.send_gravitybox_grabbed_packet(c_id, p->boxid);
        }
    }
                              break;
    case CS_GRAVITYBOX_DROPPED: {
        CS_GRAVITYBOX_DROPPED_PACKET* p = reinterpret_cast<CS_GRAVITYBOX_DROPPED_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_gravitybox_lock);
            gravitybox[p->boxid].isGrabbed = p->isGrabbed;
        }
        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == c_id) continue;
            cl.send_gravitybox_dropped_packet(c_id, p->boxid);
        }
    }
                              break;
    case CS_TIME_CHANGE: {
        CS_TIME_CHANGE_PACKET* p = reinterpret_cast<CS_TIME_CHANGE_PACKET*>(packet);

        if (clients[c_id].m_ture_chaser_false_runner) {//체이서일때
            TIMER_EVENT event{ c_id,std::chrono::system_clock::now() + std::chrono::seconds(CHASER_TIME_CHANGE_COOLTIME),EV_TIME_CHANGE,0 };
            timer_queue.push(event);

            {
                std::lock_guard<std::mutex> updatelock(clients[c_id].m_container_lock);
                clients[c_id].can_time_change = false;
                clients[c_id].m_time = p->time;
            }
            for (auto& cl : clients) {
                if (cl.m_state == ST_FREE) break;
                //if (cl.m_id == c_id) continue;
                cl.send_player_time_change_packet(c_id);
            }

        }
        else {  //러너일때
            TIMER_EVENT event{ c_id,std::chrono::system_clock::now() + std::chrono::seconds(RUNNER_TIME_CHANGE_COOLTIME),EV_TIME_CHANGE,0 };
            timer_queue.push(event);

            {
                std::lock_guard<std::mutex> updatelock(clients[c_id].m_container_lock);
                clients[c_id].can_time_change = false;
                clients[c_id].m_time = p->time;
            }
            for (auto& cl : clients) {
                if (cl.m_state == ST_FREE) break;
                //if (cl.m_id == c_id) continue;
                cl.send_player_time_change_packet(c_id);
            }
        }
        break;
    }
    case CS_GRAVITYBOX_TIME_STATE: {
        CS_GRAVITYBOX_TIME_STATE_PACKET* p = reinterpret_cast<CS_GRAVITYBOX_TIME_STATE_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_gravitybox_lock);
            gravitybox[p->boxid].timestate = p->my_time;

            gravitybox[p->boxid].location.x = p->location.x;
            gravitybox[p->boxid].location.y = p->location.y;
            gravitybox[p->boxid].location.z = p->location.z;

            gravitybox[p->boxid].rotation.x = p->rotation.x;
            gravitybox[p->boxid].rotation.y = p->rotation.y;
            gravitybox[p->boxid].rotation.z = p->rotation.z;
        }
        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == c_id) continue;
            cl.send_gravitybox_time_state_packet(c_id, p->boxid);
        }
    }
                                 break;
    case CS_TEAM_CHANGE: {
        CS_TEAM_CHANGE_PACKET* p = reinterpret_cast<CS_TEAM_CHANGE_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_container_lock);

        }
    }
                       break;
    case CS_TAKE_PICTURE: {
        CS_TAKE_PICTURE_PACKET* p = reinterpret_cast<CS_TAKE_PICTURE_PACKET*>(packet);
        {
            std::lock_guard<std::mutex> updatelock(clients[c_id].m_container_lock);
            clients[c_id].m_score = p->score;
            clients[c_id].m_cantakepicture = false;
        }

        TIMER_EVENT event{ c_id,std::chrono::system_clock::now() + std::chrono::seconds(TAKE_PICTURE_COOLTIME),EV_CAN_TAKE_PICTURE,0 };
        timer_queue.push(event);

        for (auto& cl : clients) {
            if (cl.m_state == ST_FREE) break;
            if (cl.m_id == c_id) continue;
            cl.send_calculate_score_packet(c_id);
        }
    }
                        break;
    }
}

int WorkerThread::get_new_gravitybox_id()
{
    for (int i = 0; i < MAX_GRAVITYBOX; ++i) {
        if (gravitybox[i].gravitybox_state == GravityBox_STATE::ST_NULL) {
            gravitybox[i].gravitybox_state = GravityBox_STATE::ST_OCCUPY;
            return i;
        }
    }
    return -1;
}