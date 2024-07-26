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
    strcpy_s(packet.nickname, clients[c_id].m_name);
    packet.time = clients[c_id].m_time;
    packet.ischaser = clients[c_id].m_ture_chaser_false_runner;

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
    strcpy_s(packet.nickname, clients[c_id].m_name);
    packet.ischaser = clients[c_id].m_ture_chaser_false_runner;
    packet.score = clients[c_id].m_score;
    packet.CanTimeChange = clients[c_id].can_time_change;
    packet.CanSpawnGravityBox = clients[c_id].m_canspawngravitybox;
    packet.CanTakePicture = clients[c_id].m_cantakepicture;
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
    packet.isLanded = clients[c_id].m_isjump;
    packet.HaveTimeMachine = clients[c_id].m_HaveTimeMachine;
    packet.DoingTimeTravel = clients[c_id].m_doingtimetravel;
    SendPacket(&packet);
}

void Session::send_gravitybox_add_packet(int c_id, int box_id)
{
    SC_GRAVITYBOX_ADD_PACKET packet;
    packet.type = SC_GRAVITYBOX_ADD;
    packet.size = sizeof SC_GRAVITYBOX_ADD_PACKET;
    packet.id = c_id;
    packet.boxid = box_id;
    packet.location.x = gravitybox[box_id].location.x;
    packet.location.y = gravitybox[box_id].location.y;
    packet.location.z = gravitybox[box_id].location.z;
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
    packet.location.x = gravitybox[box_id].location.x;
    packet.location.y = gravitybox[box_id].location.y;
    packet.location.z = gravitybox[box_id].location.z;
    packet.rotation.x = gravitybox[box_id].rotation.x;
    packet.rotation.y = gravitybox[box_id].rotation.y;
    packet.rotation.z = gravitybox[box_id].rotation.z;
    packet.velocity.x = gravitybox[box_id].velocity.x;
    packet.velocity.y = gravitybox[box_id].velocity.y;
    packet.velocity.z = gravitybox[box_id].velocity.z;
    packet.time = gravitybox[box_id].time;
    packet.grabbed_time = gravitybox[box_id].grabbed_time;
    packet.ismoving = gravitybox[box_id].IsMoving;
    packet.Canfixpos = gravitybox[box_id].CanFixPos;
    packet.canfall = gravitybox[box_id].CanFall;

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

void Session::send_gravitybox_time_state_packet(int c_id, int box_id)
{
    SC_GRAVITYBOX_TIME_STATE_PACKET packet;
    packet.type = SC_GRAVITYBOX_TIME_STATE;
    packet.size = sizeof SC_GRAVITYBOX_TIME_STATE_PACKET;
    packet.boxid = box_id;
    packet.timestate = gravitybox[box_id].timestate;
    packet.location.x = gravitybox[box_id].location.x;
    packet.location.y = gravitybox[box_id].location.y;
    packet.location.z = gravitybox[box_id].location.z;
    packet.rotation.x = gravitybox[box_id].rotation.x;
    packet.rotation.y = gravitybox[box_id].rotation.y;
    packet.rotation.z = gravitybox[box_id].rotation.z;
    SendPacket(&packet);
}

void Session::send_game_time_packet(int s)
{
    SC_GAME_TIMER_ON_PACKET packet;
    packet.type = SC_GAME_TIMER;
    packet.size = sizeof SC_GAME_TIMER_ON_PACKET;
    packet.seconds = s;
    SendPacket(&packet);
}

void Session::send_team_change_packet(int c_id)
{
    SC_TEAM_CHANGE_PACKET packet;
    packet.type = SC_TEAM_CHANGE;
    packet.size = sizeof SC_TEAM_CHANGE_PACKET;
    packet.ischaser = clients[c_id].m_ture_chaser_false_runner;
    SendPacket(&packet);
}

void Session::send_calculate_score_packet(int c_id)
{
    SC_CALCULATE_SCORE_PACKET packet;
    packet.type = SC_CALCULATE_SCORE;
    packet.size = sizeof SC_CALCULATE_SCORE_PACKET;
    packet.id = c_id;
    packet.score = clients[c_id].m_score;
    SendPacket(&packet);
}

void Session::send_can_take_picture_packet(int c_id)
{
    SC_CAN_TAKE_PICTURE_PACKET packet;
    packet.type = SC_CAN_TAKE_PICTURE;
    packet.size = sizeof SC_CAN_TAKE_PICTURE_PACKET;
    packet.cantakepicture = clients[c_id].m_cantakepicture;
    SendPacket(&packet);
}

void Session::send_can_spawn_gravitybox(int c_id)
{
    SC_CAN_SPAWN_GRAVITYBOX_PACKET packet;
    packet.type = SC_CAN_SPAWN_GRAVITYBOX;
    packet.size = sizeof SC_CAN_SPAWN_GRAVITYBOX_PACKET;
    packet.canspawngravitybox = clients[c_id].m_canspawngravitybox;
    SendPacket(&packet);
}

void Session::send_can_time_change_packet(int c_id)
{
    SC_CAN_TIME_CHANGE_PACKET packet;
    packet.type = SC_CAN_TIME_CHANGE;
    packet.size = sizeof SC_CAN_TIME_CHANGE_PACKET;
    packet.cantimechange = clients[c_id].can_time_change;
    SendPacket(&packet);
}

void Session::send_game_end_packet()
{
    SC_GAME_END_PACKET packet;
    packet.type = SC_GAME_END;
    packet.size = sizeof SC_GAME_END_PACKET;
    SendPacket(&packet);
}

void Session::reset_player_info(int c_id)
{
    //clients[c_id].m_state_lock.lock();
    clients[c_id].m_state = ST_FREE;
    clients[c_id].m_id = -99;
    memset(clients[c_id].m_name, 0,sizeof clients[c_id].m_name);
    clients[c_id].m_location.x = 0;
    clients[c_id].m_location.y = 0;
    clients[c_id].m_location.z = 0;
    clients[c_id].m_velocity.x = 0;
    clients[c_id].m_velocity.y = 0;
    clients[c_id].m_velocity.z = 0;
    clients[c_id].m_yaw = 0;
    clients[c_id].m_time = 0;
    clients[c_id].m_score = 0;
    clients[c_id].m_prev_remain_data = 0;
    clients[c_id].m_isjump = false;
    clients[c_id].can_time_change = true;
    clients[c_id].m_canspawngravitybox = true;
    clients[c_id].m_cantakepicture = true;
  //  clients[c_id].m_state_lock.unlock();
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

void reset_gravitybox_info()
{
    for (auto& box : gravitybox) {
        if (box.gravitybox_state == GravityBox_STATE::ST_OCCUPY) {
            box.gravitybox_state = GravityBox_STATE::ST_NULL;
            box.BoxId = -1;
            box.ByWhoId = -1;
            box.CanFall = false;
            box.CanFixPos = false;
            box.grabbed_time = -1;
            box.isGrabbed = false;
            box.isGrabbed = false;
            box.IsMoving = false;
            box.location.x = 0;
            box.location.y = 0;
            box.location.z = 0;
            box.rotation.x = 0;
            box.rotation.y = 0;
            box.rotation.z = 0;
            box.time = -1;
            box.timestate = -1;
            box.velocity.x = 0;
            box.velocity.y = 0;
            box.velocity.z = 0;
        }
        else break;
    }
}
