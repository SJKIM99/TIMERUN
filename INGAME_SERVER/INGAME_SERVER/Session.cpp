#include "pch.h"
#include "Session.h"

std::array<Session, MAX_USER> clients;

std::vector<std::queue<Session>> channels(10);

void Session::send_add_player_packet(int c_id)
{
	SC_ADD_PLAYER_PACKET packet;
	packet.id = c_id;
	packet.size = sizeof SC_ADD_PLAYER_PACKET;
	packet.type = SC_ADD_PLAYER;
	SendPacket(&packet);
}

void Session::send_ingame_login_sucess_packet(int c_id)
{
	SC_INGAME_SUCCESS_PACKET packet;
	packet.id = c_id;
	packet.size = sizeof SC_INGAME_SUCCESS_PACKET;
	packet.type = SC_INGAME_SUCCESS;
	packet.location.x = clients[c_id].m_location.x;
	packet.location.y = clients[c_id].m_location.y;
	packet.location.z = clients[c_id].m_location.z;
	packet.velocity.x = clients[c_id].m_velocity.x;
	packet.velocity.y = clients[c_id].m_velocity.y;
	packet.velocity.z = clients[c_id].m_velocity.z;
	packet.yaw = clients[c_id].m_yaw;

	SendPacket(&packet);
}
void Session::send_move_packet(int c_id)
{
	SC_MOVE_PACKET packet;
	packet.id = c_id;
	packet.size = sizeof SC_MOVE_PACKET;
	packet.type = SC_MOVE_PLAYER;
	packet.location.x = clients[c_id].m_location.x;
	packet.location.y = clients[c_id].m_location.y;
	packet.location.z = clients[c_id].m_location.z;
	packet.yaw = clients[c_id].m_yaw;
	SendPacket(&packet);
}

void Session::send_world_update_packet(int c_id)
{
	SC_WORLD_UPDATE_PACKET packet;
	packet.id = c_id;
	packet.size = sizeof SC_WORLD_UPDATE_PACKET;
	packet.type = SC_WORLD_UPDATE;
	packet.location.x = clients[c_id].m_location.x;
	packet.location.y = clients[c_id].m_location.y;
	packet.location.z = clients[c_id].m_location.z;
	packet.velocity.x = clients[c_id].m_velocity.x;
	packet.velocity.y = clients[c_id].m_velocity.y;
	packet.velocity.z = clients[c_id].m_velocity.z;
	packet.yaw = clients[c_id].m_yaw;
	SendPacket(&packet);
}


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
