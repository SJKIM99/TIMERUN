#include "pch.h"
#include "WorkerThread.h"

OVER_EXP g_over;

void WorkerThread::woker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		//WSAOVERLAPPED* over = nullptr;
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
			ZeroMemory(&g_over.over, sizeof(g_over.over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_server_socket, g_client_socket, g_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_over.over);
		}
					  break;
		case OP_RECV: {
			int remain_data = num_bytes + clients[key].m_prev_remain_data;
			char* p = ex_over->send_buf;
			while (remain_data > 0) {
				if (remain_data < sizeof(int)) {
					break;
				}
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					ProcessPakcet(static_cast<int>(key), p);
					p += packet_size;
					remain_data -= packet_size;
				}
				else {
					break;
				}
			}
		}
					break;
		case OP_SEND: {
			delete ex_over;
		}
					break;
		}
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

void WorkerThread::ProcessPakcet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_SELECT_CHANNEL: {//선택한 채널을 이어주고, 해당채널에 ID를 설정한다.
		CS_SELECT_CHANNEL_PACKET* p = reinterpret_cast<CS_SELECT_CHANNEL_PACKET*>(packet);
		int selected_channel = p->channel;
		clients[c_id].m_channel = selected_channel;
		clients[c_id].m_client_in_channel_id = get_new_client_in_channel_id();

		if (selected_channel >= 0 && selected_channel < channels.size()) {
			channels[selected_channel].push(clients[c_id]);
		}
	}
						  break;
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		/*	clients[c_id].m_location = p->location;
			clients[c_id].m_yaw = p->yaw;*/

		int client_channel = clients[c_id].m_channel;
		int client_in_channel_id = clients[c_id].m_client_in_channel_id;

		if (client_channel >= 0 && client_channel < channels.size()) {
			std::queue<Session>& channel_queue = channels[client_channel];
			for (int i = 0; i < channel_queue.size(); ++i) {
				Session& client_in_channel = channel_queue.front();
				if (client_in_channel.m_id == client_in_channel_id) {
					client_in_channel.m_location = p->location;
					client_in_channel.m_yaw = p->yaw;

					switch (p->direction) {
					case direction::right: {

					}
					case direction::left: {

					}
					case direction::forward: {

					}
					case direction::back: {

					}
										break;
					}
				}
				channel_queue.push(client_in_channel);
				channel_queue.pop();
			}
		}
	}
				break;
	}
}
