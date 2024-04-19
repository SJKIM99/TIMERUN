#pragma once

#define	BUF_SIZE		8192
#define MAX_CLIENTS		100
#define LOGIN_SERVERIP "25.52.149.86"

#define INGAME_SERVERIP "25.58.12.4"

//#define LOGIN_SERVERIP "127.0.0.1"
//
//#define INGAME_SERVERIP "127.0.0.1"

enum COMP_TYPE {
	OP_ACCEPT,
	OP_RECV, OP_SEND, 
	OP_WORLD_UPDATE
};

enum direction { forward, back, left, right };

struct vector_d3 {
	float x;
	float y;
	float z;

	vector_d3() : x(-1.f), y(-1.f), z(-1.f) {}
	vector_d3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

constexpr int NAMESIZE = 20;
constexpr int IDSIZE = 20;
constexpr int PASSWDSIZE = 20;
constexpr int IPSIZE = 20;

constexpr int MAX_USER = 10;
constexpr int MAX_GRAVITYBOX = 100;

constexpr int TIMESIZE = 10;


//C2S
constexpr char CS_LOGIN = 0;
constexpr char CS_SIGNUP = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_SELECT_CHANNEL = 3;
constexpr char CS_INGAME_LOGIN = 4;
constexpr char CS_PLAYER_UPDATE= 5;
constexpr char CS_GRAVITYBOX_ADD= 6;
constexpr char CS_GRAVITYBOX_UPDATE= 7;
constexpr char CS_PLAYER_JUMP= 8;
constexpr char CS_GRAVITYBOX_GRABBED= 9;
constexpr char CS_GRAVITYBOX_DROPPED = 10;
constexpr char CS_TIME_CHANGE = 11;

//S2C
constexpr char SC_LOGIN_SUCCESS = 1;
constexpr char SC_LOGIN_FAIL = 2;
constexpr char SC_SIGNUP = 3;
constexpr char SC_SIGNUP_SUCCESS = 4;
constexpr char SC_MOVE_PLAYER = 5;
constexpr char SC_ADD_PLAYER = 6;
constexpr char SC_INGAME_SUCCESS = 7;
constexpr char SC_WORLD_UPDATE = 8;
constexpr char SC_GRAVITYBOX_ADD = 9;
constexpr char SC_GRAVITYBOX_UPDATE = 10;
constexpr char SC_PLAYER_JUMP = 11;
constexpr char SC_GRAVIRTBOX_GRABBED = 12;
constexpr char SC_GRAVIRTBOX_DROPPED = 13;
constexpr char SC_TIME_CHANGE = 14;


#pragma pack (push, 1)

//C2S
struct CS_LOGIN_PACKET {
	unsigned char size;
	char type;
	char id[IDSIZE];
	char passwd[PASSWDSIZE];
};

struct CS_SIGNUP_PACKET {
	unsigned char size;
	char type;
	char id[IDSIZE];
	char passwd[PASSWDSIZE];
	char nickname[NAMESIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char type;
	int id;
	direction direction;
	double yaw;
	vector_d3 location;
};

struct CS_SELECT_CHANNEL_PACKET {
	unsigned char size;
	char type;
	int channel;
};

struct CS_INGAME_LOGIN_PACKET {
	unsigned char size;
	char type;
};

struct CS_PLAYER_UPDATE_PACKET {
	unsigned char size;
	char type;
	int id;
	double yaw;
	vector_d3 location;
	vector_d3 velocity;
	bool HaveGravityGun;
	int time;
};

struct CS_GRAVITYBOX_ADD_PACKET {
	unsigned char size;
	char type;
	int id;
	vector_d3 location;
	vector_d3 rotation;
	int time;
};

struct CS_GRAVITYBOX_UPDATE_PACKET {
	unsigned char size;
	char type;
	int bywhoid;
	int boxid;
	bool isgrabbed;
	vector_d3 location;
	vector_d3 rotation;
	vector_d3 velocity;
	int time;
	int grabbed_time;
};

struct CS_PLAYER_JUMP_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct CS_GRAVITYBOX_GRABBED_PACKET {
	unsigned char size;
	char type;
	int boxid;
	bool isGrabbed;
};

struct CS_GRAVITYBOX_DROPPED_PACKET {
	unsigned char size;
	char type;
	int boxid;
	bool isGrabbed;
};

struct CS_TIME_CHANGE_PACKET {
	unsigned char size;
	char type;
	int id;
	int time;
};
//S2C
struct SC_SIGNUP_PACKET {
	unsigned char size;
	char type;
};

struct SC_SIGNUP_SUCCESS_PACKET {
	unsigned char size;
	char type;
};

struct SC_LOGIN_SUCCESS_PACKET {
	unsigned char size;
	char type;
	char nickname[NAMESIZE];
};

struct SC_MOVE_PACKET {
	unsigned char size;
	char type;
	int	 id;
	double yaw;
	vector_d3 location;
};

struct SC_LOGIN_FAIL {
	unsigned char size;
	char type;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char type;
	int id;
	vector_d3 location;
};

struct SC_INGAME_SUCCESS_PACKET {
	unsigned char size;
	char type;
	int id;
	vector_d3 location;
	vector_d3 velocity;
	float yaw;
};

struct SC_WORLD_UPDATE_PACKET {
	unsigned char size;
	char type;
	int id;
	double yaw;
	vector_d3 location;
	vector_d3 velocity;
	bool HaveGravityGun;
};

struct SC_GRAVITYBOX_ADD_PACKET {
	unsigned char size;
	char type;
	int id;
	int boxid;
	vector_d3 location;
	vector_d3 rotation;
	int box_time;
};

struct SC_GRAVITYBOX_UPDATE_PACKET {
	unsigned char size;
	char type;
	int id;
	int boxid;
	bool isGrabbed;
	vector_d3 location;
	vector_d3 rotation;
	vector_d3 velocity;
	int time;
	int grabbed_time;
};

struct SC_PLAYER_JUMP_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct SC_GRAVIRTBOX_GRABBED_PACKET {
	unsigned char size;
	char type;
	int id;
	int box_id;
	bool isGrabbed;
};

struct SC_GRAVIRTBOX_DROPPED_PACKET {
	unsigned char size;
	char type;
	int id;
	int box_id;
	bool isGrabbed;
};

struct SC_TIME_CHANGE_PACKET {
	unsigned char size;
	char type;
	int id;
	int time;
};
#pragma pack(pop)