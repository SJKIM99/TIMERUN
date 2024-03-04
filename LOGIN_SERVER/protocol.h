#pragma once

#define	BUF_SIZE		2048
#define MAX_CLIENTS		100
#define LOGIN_SERVERIP "25.52.149.86"

#define INGAME_SERVERIP "25.58.12.4"

//#define LOGIN_SERVERIP "127.0.0.1"
//
//#define INGAME_SERVERIP "127.0.0.1"

enum COMP_TYPE {
	OP_ACCEPT,
	OP_RECV, OP_SEND,
};

enum direction { forward, back, left, right };

struct vector_d3 {
	double x;
	double y;
	double z;
	
	vector_d3() : x(-1.f), y(-1.f), z(-1.f) {}
	vector_d3(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

constexpr int NAMESIZE = 20;
constexpr int IDSIZE = 20;
constexpr int PASSWDSIZE = 20;
constexpr int IPSIZE = 20;

constexpr int MAX_USER = 3000;
constexpr int MAX_CHANNEL_USER = 300;

//C2S
constexpr char CS_LOGIN = 0;
constexpr char CS_SIGNUP = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_SELECT_CHANNEL = 3;
constexpr char CS_INGAME_LOGIN = 4;

//S2C
constexpr char SC_LOGIN_SUCCESS = 1;
constexpr char SC_LOGIN_FAIL = 2;
constexpr char SC_SIGNUP = 3;
constexpr char SC_SIGNUP_SUCCESS = 4;
constexpr char SC_MOVE_PLAYER = 5;
constexpr char SC_ADD_PLAYER = 6;
constexpr char SC_INGAME_SUCCESS = 7;


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
	//direction direction;
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
};

struct SC_INGAME_SUCCESS_PACKET {
	unsigned char size;
	char type;
	int id;
};

#pragma pack(pop)