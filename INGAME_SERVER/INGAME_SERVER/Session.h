#pragma once

#include <mutex>

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

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
enum GravityBox_STATE { ST_NULL, ST_OCCUPY };

struct GravityBox {
    int ByWhoId;
    int BoxId;
    bool isGrabbed;
    GravityBox_STATE gravitybox_state;
    vector_d3 location;
    vector_d3 rotation;
    vector_d3 velocity;
    int time;
    int grabbed_time;
    bool IsMoving;
    bool CanFixPos;
    bool CanFall;
    int timestate;
};

void reset_gravitybox_info();

class Session
{
    OVER_EXP over;
public:
    SOCKET m_socket;
    S_STATE m_state;
    std::mutex m_container_lock;
    std::mutex m_state_lock;
    std::mutex m_channel_lock;
    std::mutex m_update_lock;
    std::mutex m_gravitybox_lock;
    std::mutex m_enter_lock;
    int m_id;
    char m_name[NAMESIZE];
    vector_d3 m_location;
    vector_d3 m_velocity;
    bool m_HaveGrabityGun;
    double m_yaw;
    int m_prev_remain_data;
    int m_time;
    bool m_isjump;
    bool m_HaveTimeMachine;
    bool m_cantakepicture;
    int m_score;
    bool m_canspawngravitybox;
    bool m_doingtimetravel;
    bool m_ture_chaser_false_runner;
    bool can_time_change;
public:
    bool m_online;
public:
    Session() {
        m_socket = 0;
        m_id = 1;
        m_name[0] = 0;
        m_prev_remain_data = 0;
        can_time_change = true;
        m_canspawngravitybox = true;
        m_cantakepicture = true;
    }

    Session(const Session& other) {
        m_socket = other.m_socket;
        m_state = other.m_state;
        m_id = other.m_id;
        strcpy_s(m_name, other.m_name);
        m_location = other.m_location;
        m_yaw = other.m_yaw;
        m_prev_remain_data = other.m_prev_remain_data;
        m_online = other.m_online;
    }

    ~Session() {}
public:
    void send_add_player_packet(int c_id);
    void send_ingame_login_sucess_packet(int c_id);
    void send_move_packet(int c_id);
    void send_world_update_packet(int c_id);
    void send_gravitybox_add_packet(int c_id, int box_id);
    void send_gravitybox_update_packet(int c_id, int box_id);
    void send_player_jump_packet(int c_id);
    void send_gravitybox_grabbed_packet(int c_id, int box_id);
    void send_gravitybox_dropped_packet(int c_id, int box_id);
    void send_player_time_change_packet(int c_id);
    void send_gravitybox_time_state_packet(int c_id, int box_id);
    void send_game_time_packet(int s);
    void send_team_change_packet(int c_id);
    void send_calculate_score_packet(int c_id);
    void send_can_take_picture_packet(int c_id);
    void send_can_spawn_gravitybox(int c_id);
    void send_can_time_change_packet(int c_id);
    void send_game_end_packet();
    void reset_player_info(int c_id);
public:
    void SendPacket(void* packet);
    void RecvPacket();

    void StateLockFunc() {
        std::lock_guard<std::mutex> state_lock(m_state_lock);
    }
    void ChannelLockFun() {
        std::lock_guard<std::mutex> channel_lock(m_channel_lock);
    }
};

extern std::array<Session, MAX_USER> clients;
extern std::array<GravityBox, MAX_GRAVITYBOX> gravitybox;

void disconnect(int c_id);
