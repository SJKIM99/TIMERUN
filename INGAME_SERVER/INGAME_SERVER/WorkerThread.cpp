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
			std::cout << client_id << "번 클라이언트가 입장했습니다." << std::endl;
			if (client_id != -1) {
				{
					std::lock_guard<std::mutex> ll{ clients[client_id].m_state_lock };
					clients[client_id].m_state = ST_ALLOC;
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
		}
	}
}

//void WorkerThread::timer_thread()
//{
//	IngameMain ingameMain{};
//
//	while (true) {
//		TIMER_EVENT event;
//		auto current_time = std::chrono::system_clock::now();
//		if (true == timer_queue.try_pop(event)) {
//			if (event.wakeup_time > current_time) {
//				timer_queue.push(event);
//				std::this_thread::sleep_for(std::chrono::milliseconds(1));
//				continue;
//			}
//			switch (event.event) {
//			}
//			continue;
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}




int WorkerThread::get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard <std::mutex> ll{ clients[i].m_state_lock };
		if (clients[i].m_state == ST_FREE)
			return i;
	}
	return -1;
}

int WorkerThread::get_new_client_in_channel_id()
{
	for (int i = 0; i < MAX_CHANNEL_USER; ++i) {
		std::lock_guard <std::mutex> ll{ clients[i].m_channel_lock };
		if (clients[i].m_client_in_channel_id == -1) {
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
	clients[player_id].m_location.x = 0;
	clients[player_id].m_location.y = 0;
	clients[player_id].m_location.z = 0;
	clients[player_id].m_yaw = 0;
	clients[player_id].m_velocity.x = 0;
	clients[player_id].m_velocity.y = 0;
	clients[player_id].m_velocity.z = 0;
	clients[player_id].m_HaveGrabityGun = false;
}

void WorkerThread::ProcessPacket(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_INGAME_LOGIN: {
		isEnterPlayer = true;
		CS_INGAME_LOGIN_PACKET* p = reinterpret_cast<CS_INGAME_LOGIN_PACKET*>(packet);
		std::cout << c_id << "번 클라이언트 인게임 로그인 성공" << std::endl;

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

			clients[p->id].m_HaveGrabityGun = p->HaveGravityGun;
	//		float vel_size = sqrt(p->velocity.x * p->velocity.x + p->velocity.y * p->velocity.y +p->velocity.z * p->velocity.z);
			clients[p->id].m_yaw = p->yaw;
		//	std::cout << p->id << "번 클라 " << " " << vel_size << std::endl;
		}
		for (auto& cl : clients) {
			if (cl.m_state == ST_FREE) break;
			if (cl.m_id == p->id) continue;
			cl.send_world_update_packet(c_id);
		}
	}
						 break;
	case CS_GRAVITYBOX_UPDATE: {
		std::cout << "중력박스 스폰?" << std::endl;
		CS_GRAVITYBOX_UPDATE_PACKET* p = reinterpret_cast<CS_GRAVITYBOX_UPDATE_PACKET*>(packet);
		{
			std::lock_guard<std::mutex> updatelock(clients[c_id].m_gravitybox_lock);
			clients[p->id].m_gravitybox_location.x = p->location.x;
			clients[p->id].m_gravitybox_location.y = p->location.y;
			clients[p->id].m_gravitybox_location.z = p->location.z;

			std::cout << p->location.x << p->location.y << p->location.z << std::endl;

			clients[p->id].m_gravitybox_rotation.x = p->rotation.x;
			clients[p->id].m_gravitybox_rotation.y = p->rotation.y;
			clients[p->id].m_gravitybox_rotation.z = p->rotation.z;
		}
		for (auto& cl : clients) {
			if (cl.m_state == ST_FREE) break;
			if (cl.m_id == p->id) continue;
			cl.send_gravitybox_update_packet(c_id);
		}
	}
							 break;
	}
}
