#include "pch.h"
#include "Session.h"

std::array<Session, MAX_USER> clients;
std::array<GravityBox, MAX_GRAVITYBOX> gravitybox;

std::vector<std::queue<Session>> channels(10);

void Session::send_add_player_packet(int c_id)
{
	SC_ADD_PLAYER_PACKET packet;
	packet.id = c_id;
	packet.size = sizeof SC_ADD_PLAYER_PACKET;
	packet.type = SC_ADD_PLAYER;
	packet.location.x = clients[c_id].m_location.x;
	packet.location.y = clients[c_id].m_location.y;
	packet.location.z = clients[c_id].m_location.z;

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
	packet.yaw = clients[c_id].m_yaw;
	packet.velocity.x = clients[c_id].m_velocity.x;
	packet.velocity.y = clients[c_id].m_velocity.y;
	packet.velocity.z = clients[c_id].m_velocity.z;

	
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
	packet.HaveGravityGun = clients[c_id].m_HaveGrabityGun;
	SendPacket(&packet);
}

void Session::send_gravitybox_add_packet(int c_id, int box_id)
{
	SC_GRAVITYBOX_ADD_PACKET packet;
	packet.type = SC_GRAVITYBOX_ADD;
	packet.size = sizeof SC_GRAVITYBOX_ADD_PACKET;
	packet.id = c_id;
	packet.boxid = box_id;
	packet.location.x = gravitybox[box_id].locaton.x;
	packet.location.y = gravitybox[box_id].locaton.y;
	packet.location.z = gravitybox[box_id].locaton.z;
	packet.rotation.x = gravitybox[box_id].rotation.x;
	packet.rotation.y = gravitybox[box_id].rotation.y;
	packet.rotation.z = gravitybox[box_id].rotation.z;
	packet.box_time = gravitybox[box_id].time;
	SendPacket(&packet);
}

void Session::send_gravitybox_update_packet(int c_id, int box_id)
{
	SC_GRAVITYBOX_UPDATE_PACKET packet;
	packet.type = SC_GRAVITYBOX_UPDATE;
	packet.size = sizeof SC_GRAVITYBOX_UPDATE_PACKET;
	packet.id = gravitybox[box_id].ByWhoId;
	packet.boxid = box_id;
	packet.isGrabbed = gravitybox[box_id].isGrabbed;
	packet.location.x = gravitybox[box_id].locaton.x;
	packet.location.y = gravitybox[box_id].locaton.y;
	packet.location.z = gravitybox[box_id].locaton.z;
	packet.rotation.x = gravitybox[box_id].rotation.x;
	packet.rotation.y = gravitybox[box_id].rotation.y;
	packet.rotation.z = gravitybox[box_id].rotation.z;
	packet.velocity.x = gravitybox[box_id].velocity.x;
	packet.velocity.y = gravitybox[box_id].velocity.y;
	packet.velocity.z = gravitybox[box_id].velocity.z;

	SendPacket(&packet);
}

void Session::send_player_jump_packet(int c_id)
{
	SC_PLAYER_JUMP_PACKET packet;
	packet.type = SC_PLAYER_JUMP;
	packet.size = sizeof SC_PLAYER_JUMP_PACKET;
	packet.id = c_id;
	SendPacket(&packet);
}

void Session::send_gravitybox_grabbed_packet(int c_id, int box_id)
{
	SC_GRAVIRTBOX_GRABBED_PACKET packet;
	packet.type = SC_GRAVIRTBOX_GRABBED;
	packet.size = sizeof SC_GRAVIRTBOX_GRABBED_PACKET;
	packet.id = c_id;
	packet.box_id = box_id;
	packet.isGrabbed = gravitybox[box_id].isGrabbed;
	SendPacket(&packet);
}

void Session::send_gravitybox_dropped_packet(int c_id, int box_id)
{
	SC_GRAVIRTBOX_DROPPED_PACKET packet;
	packet.type = SC_GRAVIRTBOX_DROPPED;
	packet.size = sizeof SC_GRAVIRTBOX_DROPPED_PACKET;
	packet.id = c_id;
	packet.box_id = box_id;
	packet.isGrabbed = gravitybox[box_id].isGrabbed;
	SendPacket(&packet);
}

void Session::send_player_time_change_packet(int c_id)
{
	SC_TIME_CHANGE_PACKET packet;
	packet.type = SC_TIME_CHANGE;
	packet.size = sizeof SC_TIME_CHANGE_PACKET;
	packet.id = c_id;
	packet.time = clients[c_id].m_time;
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
