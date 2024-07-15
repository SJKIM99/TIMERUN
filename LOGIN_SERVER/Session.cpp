#include "pch.h"
#include "Session.h"

std::array<Session, MAX_USER> clients;

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

void Session::send_signup_packet()
{
    SC_SIGNUP_PACKET packet;
    packet.size = sizeof SC_SIGNUP_PACKET;
    packet.type = SC_SIGNUP;
    SendPacket(&packet);
}

void Session::send_signup_success_packet()
{
    SC_SIGNUP_SUCCESS_PACKET packet;
    packet.size = sizeof SC_SIGNUP_SUCCESS_PACKET;
    packet.type = SC_SIGNUP_SUCCESS;
    SendPacket(&packet);
}

void Session::send_login_success_packet(PlayerInfo playerinfo)
{
    SC_LOGIN_SUCCESS_PACKET packet;
    packet.size = sizeof SC_LOGIN_SUCCESS_PACKET;
    packet.type = SC_LOGIN_SUCCESS;
    strcpy_s(packet.nickname, playerinfo.player_nickname);
    SendPacket(&packet);
}

void Session::send_login_fail_packet()
{
    SC_LOGIN_FAIL_PACKET packet;
    packet.size = sizeof SC_LOGIN_FAIL_PACKET;
    packet.type = SC_LOGIN_FAIL;
    SendPacket(&packet);

}

void Session::send_signup_fail_packet()
{
    SC_SIGNUP_FAIL_PACKET packet;
    packet.size = sizeof SC_SIGNUP_FAIL_PACKET;
    packet.type = SC_SIGNUP_FAIL;
    SendPacket(&packet);
}

void Session::send_ready_packet(int c_id)
{
    SC_READY_PACKET packet;
    packet.size = sizeof SC_READY_PACKET;
    packet.type = SC_READY;
    packet.ready = clients[c_id].m_ready;
    strcpy_s(packet.nickname, clients[c_id].m_name);
    SendPacket(&packet);
}

void Session::send_all_player_ready_packet()
{
    SC_ALL_PLAYER_READY_PACKET packet;
    packet.size = sizeof SC_ALL_PLAYER_READY_PACKET;
    packet.type = SC_ALL_PLAYER_READY;
    SendPacket(&packet);
}

void Session::send_game_start_packet(int c_id)
{
    SC_GAME_START_PACKET packet;
    packet.size = sizeof SC_GAME_START_PACKET;
    packet.type = SC_GAME_START;
    // packet.ischaser = clients[c_id].m_ischaser;
    SendPacket(&packet);
}

void disconnect(int c_id)
{

}