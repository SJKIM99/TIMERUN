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
				}
				InitPlayerInfo(client_id);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_client_socket),h_iocp, client_id, 0);
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

void WorkerThread::InitPlayerInfo(int player_id)
{
	clients[player_id].m_socket = g_client_socket;
	clients[player_id].m_id = player_id;
	clients[player_id].m_name[0] = 0;
	clients[player_id].m_prev_remain_data = 0;
	clients[player_id].m_online = true;
}

void WorkerThread::ProcessPakcet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (dbThreadInstance->GetDatabase()->IsIDRegistered(p->id) && dbThreadInstance->GetDatabase()->IsPASSWDRegistered(p->passwd)) {
			//여기서 클라이언트에게 다시 session정보와 인게임서버토큰을 전송한다
			clients[c_id].send_login_success_packet(dbThreadInstance->GetDatabase()->ExtractPlayerInfo(p->id, p->passwd));
		}
		else {
			clients[c_id].send_signup_packet();
		}
	}
				 break;
	case CS_SIGNUP: {
		CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);
		if (dbThreadInstance->GetDatabase()->RegisteredAccount(c_id, p->id, p->passwd, p->nickname)) {
			clients[c_id].send_signup_success_packet();
		}
		else {

		}
	}
				  break;
	}
}
