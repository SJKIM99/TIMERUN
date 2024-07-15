#pragma once

#define   BUF_SIZE      8192
#define MAX_CLIENTS      100
//#define LOGIN_SERVERIP "25.52.149.86"   //하마치 내주소
#define LOGIN_SERVERIP "210.99.254.132"    //포트포워딩주소

//#define INGAME_SERVERIP "25.58.12.4"
#define INGAME_SERVERIP "210.99.254.132"  //포트포워딩주소

//#define LOGIN_SERVERIP "127.0.0.1"
//
//#define INGAME_SERVERIP "127.0.0.1"

enum COMP_TYPE {
    OP_ACCEPT,
    OP_RECV, OP_SEND,
    OP_GAME_START, OP_TEAM_CHANGE, OP_CAN_TAKE_PICTURE,
    OP_CAN_SPAWN_GRAVITYBOX, OP_GAME_TIMER_ON
};

enum TIMER_EVENT_TYPE { EV_GAME_START, EV_TEAM_CHANGE, EV_CAN_TAKE_PICTURE, EV_CAN_SPAWN_GRAVITYBOX, EV_GAME_TIMER_ON };

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

constexpr int MAX_USER = 1000;
constexpr int MAX_GRAVITYBOX = 100;

constexpr int TIMESIZE = 11;

constexpr int PLAYTIME = 5;

constexpr int TAKE_PICTURE_COOLTIME = 3;
constexpr int SPAWN_GRAVITYBOX_COOLTIME = 15;
constexpr int GMAE_START_COOLTIME = 6;


//C2S
constexpr char CS_LOGIN = 0;
constexpr char CS_SIGNUP = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_SELECT_CHANNEL = 3;
constexpr char CS_INGAME_LOGIN = 4;
constexpr char CS_PLAYER_UPDATE = 5;
constexpr char CS_GRAVITYBOX_ADD = 6;
constexpr char CS_GRAVITYBOX_UPDATE = 7;
constexpr char CS_PLAYER_JUMP = 8;
constexpr char CS_GRAVITYBOX_GRABBED = 9;
constexpr char CS_GRAVITYBOX_DROPPED = 10;
constexpr char CS_TIME_CHANGE = 11;
constexpr char CS_READY = 12;
constexpr char CS_GRAVITYBOX_TIME_STATE = 13;
constexpr char CS_TEAM_CHANGE = 14;
constexpr char CS_TAKE_PICTURE = 15;

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
constexpr char SC_SIGNUP_FAIL = 15;
constexpr char SC_READY = 16;
constexpr char SC_GAME_START = 17;
constexpr char SC_GRAVITYBOX_TIME_STATE = 18;
constexpr char SC_TEAM_CHANGE = 19;
constexpr char SC_CALCULATE_SCORE = 20;
constexpr char SC_CAN_TAKE_PICTURE = 21;
constexpr char SC_CAN_SPAWN_GRAVITYBOX = 22;
constexpr char SC_ALL_PLAYER_READY = 23;
constexpr char SC_GAME_TIMER = 24;
constexpr char SC_CHANGE_ATTACK_DEFFENSE = 25;

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
    char nickname[NAMESIZE];
    int my_time;
    vector_d3 location;
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
    bool isLanded;
    bool HaveTimeMachine;
    bool DoingTimeTravel;
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
    bool ismoving;
    bool canfixpos;
    bool canfall;
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

struct CS_READY_PACKET {
    unsigned char size;
    char type;
};

struct CS_GRAVITYBOX_TIME_STATE_PACKET {
    unsigned char size;
    char type;
    int boxid;
    int my_time;
    vector_d3 location;
    vector_d3 rotation;
};

struct CS_TEAM_CHANGE_PACKET {
    unsigned char size;
    char type;
};

struct CS_TAKE_PICTURE_PACKET {
    unsigned char size;
    char type;
    int score;
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
    int    id;
    double yaw;
    vector_d3 location;
};

struct SC_LOGIN_FAIL_PACKET {
    unsigned char size;
    char type;
};

struct SC_ADD_PLAYER_PACKET {
    unsigned char size;
    char type;
    int id;
    vector_d3 location;
    char nickname[NAMESIZE];
    int time;
};

struct SC_INGAME_SUCCESS_PACKET {
    unsigned char size;
    char type;
    int id;
    vector_d3 location;
    vector_d3 velocity;
    float yaw;
    char nickname[NAMESIZE];
    bool ischaser;
};

struct SC_WORLD_UPDATE_PACKET {
    unsigned char size;
    char type;
    int id;
    double yaw;
    vector_d3 location;
    vector_d3 velocity;
    bool HaveGravityGun;
    bool isLanded;
    bool HaveTimeMachine;
    bool DoingTimeTravel;
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
    bool ismoving;
    bool Canfixpos;
    bool canfall;
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
    bool timechangestart;
};

struct SC_SIGNUP_FAIL_PACKET {
    unsigned char size;
    char type;
};

struct SC_READY_PACKET {
    unsigned char size;
    char type;
    int id;
    bool ready;
    char nickname[NAMESIZE];
};

struct SC_GAME_START_PACKET {
    unsigned char size;
    char type;
    bool ischaser;
};

struct SC_GRAVITYBOX_TIME_STATE_PACKET {
    unsigned char size;
    char type;
    int boxid;
    int timestate;
    vector_d3 location;
    vector_d3 rotation;
};

struct SC_GAME_TIMER_ON_PACKET {
    unsigned char size;
    char type;
    int minutes;
    int seconds;
};

struct SC_CHANGE_ATTACK_DEFFENSE_PACKET {
    unsigned char size;
    char type;
    int seconds;
};

struct SC_TEAM_CHANGE_PACKET {
    unsigned char size;
    char type;
    bool ischaser;
    int time;
};

struct SC_CALCULATE_SCORE_PACKET {
    unsigned char size;
    char type;
    int id;
    int score;
};

struct SC_CAN_TAKE_PICTURE_PACKET {
    unsigned char size;
    char type;
    bool cantakepicture;
};

struct SC_CAN_SPAWN_GRAVITYBOX_PACKET {
    unsigned char size;
    char type;
    bool canspawngravitybox;
};

struct SC_ALL_PLAYER_READY_PACKET {
    unsigned char size;
    char type;
};
#pragma pack(pop)