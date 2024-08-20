#pragma once
#include "DataBase.h"

class OVER_EXP {
public:
    WSAOVERLAPPED over;
    WSABUF wsabuf;
    char send_buf[BUF_SIZE];
    COMP_TYPE comp_type;

    OVER_EXP() {
        wsabuf.buf = send_buf;
        wsabuf.len = BUF_SIZE;
        comp_type = OP_RECV;
        ZeroMemory(&over, sizeof(over));
    }

    OVER_EXP(char* packet) {
        wsabuf.buf = send_buf;
        wsabuf.len = packet[0];
        ZeroMemory(&over, sizeof(over));
        comp_type = OP_SEND;
        memcpy(send_buf, packet, packet[0]);
    }
};

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME,ST_GAME_END };

class Session
{
    OVER_EXP over;
public:
    SOCKET m_socket;
    S_STATE m_state;
    std::mutex m_state_lock;
    std::mutex m_ready_lock;
    int m_id;
    char m_name[NAMESIZE];
    bool m_ready = false;
    int m_prev_remain_data;
    // bool m_ischaser;
public:
    bool m_online;

public:
    Session() {
        m_socket = 0;
        m_id = 1;
        m_name[0] = 0;
        m_prev_remain_data = 0;
    }
    ~Session() {}
public:
    void send_signup_packet();
    void send_signup_success_packet();
    void send_login_success_packet(PlayerInfo playerinfo);
    void send_login_fail_packet();
    void send_signup_fail_packet();
    void send_ready_packet(int c_id);
    void send_all_player_ready_packet();
    void send_game_start_packet(int c_id);
public:
    void SendPacket(void* packet);
    void RecvPacket();

};

extern std::array<Session, MAX_USER> clients;

void disconnect(int c_id);