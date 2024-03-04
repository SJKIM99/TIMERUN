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

void WorkerThread::timer_thread()
{
	IngameMain ingameMain{};

	while (true) {
		TIMER_EVENT event;
		auto current_time = std::chrono::system_clock::now();
		if (true == timer_queue.try_pop(event)) {
			if (event.wakeup_time > current_time) {
				timer_queue.push(event);
				continue;
			}
			switch (event.event) {
			
			}
			continue;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void WorkerThread::world_update_thread()
{
	SC_MOVE_PACKET packet;
	packet.size = sizeof SC_MOVE_PACKET;
	packet.type = SC_MOVE;

	std::chrono::steady_clock::time_point last_send_time = std::chrono::steady_clock::now();
	while (true) {
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_send_time).count();

		if (elapsed_time >= 1000 / 33) {
			for (auto& cl : clients) {
				if (cl.m_state == ST_FREE) continue;
				packet.id = cl.m_id;
				packet.location.x = cl.m_location.x;
				packet.location.y = cl.m_location.y;
				packet.location.z = cl.m_location.z;
				packet.yaw = cl.m_yaw;
			}
			for (auto& cl : clients) {
				std::lock_guard<std::mutex> lock(cl.m_container_lock);
				if (cl.m_state != ST_FREE) {
					cl.SendPacket(&packet);
				}
			}
		}
		last_send_time = current_time;
	}
}

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
	clients[player_id].m_client_in_channel_id = -1;
}

void WorkerThread::ProcessPacket(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_INGAME_LOGIN: {
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
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);

		{
			std::lock_guard<std::mutex> movelock(clients[c_id].m_container_lock);
			clients[p->id].m_location.x = p->location.x;
			clients[p->id].m_location.y = p->location.y;
			clients[p->id].m_location.z = p->location.z;

			clients[p->id].m_yaw = p->yaw;
		}
		/*for (auto& pl : clients) {
			if (pl.m_state != ST_ALLOC) break;
			if (pl.m_id == c_id) continue;
			pl.send_move_packet(c_id);
		}
		std::cout << p->id << "번 클라이언트 이동" << std::endl;*/
	}
				break;
	}
}
