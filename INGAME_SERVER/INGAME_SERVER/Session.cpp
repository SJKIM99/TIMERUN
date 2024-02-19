#include "pch.h"
#include "Session.h"

std::array<Session, MAX_USER> clients;

std::vector<std::queue<Session>> channels(10);

void Session::SendPacket(void* packet)
{
	OVER_EXP* SendData = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(m_socket, &SendData->wsabuf, 1, 0, 0, &SendData->over, 0);
}

void Session::RecvPacket()
{
	DWORD recv_flag = 0;
	memset(&over.over, 0, sizeof(over.over));
	over.wsabuf.len = BUF_SIZE - m_prev_remain_data;
	over.wsabuf.buf = over.send_buf + m_prev_remain_data;
	WSARecv(m_socket, &over.wsabuf, 1, 0, &recv_flag,
		&over.over, 0);
}

void disconnect(int c_id)
{
}
