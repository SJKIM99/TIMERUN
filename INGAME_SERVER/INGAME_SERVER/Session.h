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
	int m_channel;
	int m_client_in_channel_id;
	int m_id;
	char m_name[NAMESIZE];
	vector_d3 m_location;
	vector_d3 m_velocity;
	bool m_HaveGrabityGun;
	double m_yaw;
	int m_prev_remain_data;
	
	//GravityBox
	int m_box_count[5];
	vector_d3 m_gravitybox_location[5];
	vector_d3 m_gravitybox_rotation[5];

public:
	bool m_online;

public:
	Session() {
		m_socket = 0;
		m_id = 1;
		m_name[0] = 0;
		m_prev_remain_data = 0;
	}

	Session(const Session& other) {
		// 여기에 필요한 멤버 변수들을 복사하는 코드를 추가하세요
		m_socket = other.m_socket;
		m_state = other.m_state;
		//m_state_lock = other.m_state_lock;
		//m_channel_lock = other.m_channel_lock;
		m_channel = other.m_channel;
		m_client_in_channel_id = other.m_client_in_channel_id;
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
extern std::vector<std::queue<Session>> channels;

void disconnect(int c_id);


