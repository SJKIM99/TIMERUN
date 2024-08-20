// Fill out your copyright notice in the Description page of Project Settings.


#include "BeginWidget.h"
#include "TIMERUNGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UBeginWidget::NativeOnInitialized()
{
    instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

}

bool UBeginWidget::SendLoginPacket()
{
    u_long blockingmode = 0;
    ioctlsocket(*instance->login_socket, FIONBIO, &blockingmode);

    CS_LOGIN_PACKET packet;
    packet.size = sizeof CS_LOGIN_PACKET;
    packet.type = CS_LOGIN;
    strcpy_s(packet.id, TCHAR_TO_ANSI(*ID));
    strcpy_s(packet.passwd, TCHAR_TO_ANSI(*PASSWD));

    send(*instance->login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

    char recv_buf[BUFSIZ];

    recv(*instance->login_socket, reinterpret_cast<char*>(recv_buf), BUFSIZ, 0);

    u_long nonblockingmode = 1;
    ioctlsocket(*instance->login_socket, FIONBIO, &nonblockingmode);

    switch (recv_buf[1]) {
    case SC_LOGIN_SUCCESS: {
        SC_LOGIN_SUCCESS_PACKET* p = reinterpret_cast<SC_LOGIN_SUCCESS_PACKET*>(recv_buf);
        NICKNAME = FString(ANSI_TO_TCHAR(p->nickname));
        auto myplayer = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        myplayer->nickname = FString(ANSI_TO_TCHAR(p->nickname));

        /*instance->socketmgr.ConnectIngameServer();
        instance->ingame_socket = instance->socketmgr.GetIngameSocket();

        CS_INGAME_LOGIN_PACKET login_packet;
        login_packet.size = sizeof CS_INGAME_LOGIN_PACKET;
        login_packet.type = CS_INGAME_LOGIN;

        int ret = send(*instance->ingame_socket, reinterpret_cast<char*>(&login_packet), sizeof(login_packet), 0);

        instance->IsActiveIngameSocket = true;*/
        return true;
    }
    case SC_LOGIN_FAIL: {
        return false;
    }
    }
    return false;
}

bool UBeginWidget::SendSignUpPacket()
{
    u_long blockingmode = 0;
    ioctlsocket(*instance->login_socket, FIONBIO, &blockingmode);

    CS_SIGNUP_PACKET packet;
    packet.size = sizeof CS_SIGNUP_PACKET;
    packet.type = CS_SIGNUP;
    strcpy_s(packet.id, TCHAR_TO_ANSI(*ID));
    strcpy_s(packet.passwd, TCHAR_TO_ANSI(*PASSWD));
    strcpy_s(packet.nickname, TCHAR_TO_ANSI(*NICKNAME));

    send(*instance->login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

    char recv_buf[BUFSIZ];

    recv(*instance->login_socket, reinterpret_cast<char*>(recv_buf), BUFSIZ, 0);

    u_long nonblockingmode = 1;
    ioctlsocket(*instance->login_socket, FIONBIO, &nonblockingmode);

    switch (recv_buf[1]) {
    case SC_SIGNUP_SUCCESS: {
        return true;
    }
    case SC_LOGIN_FAIL: {
        return false;
    }
    }
    return false;
}

void UBeginWidget::SendReadyPacket()
{
    u_long blockingmode = 0;
    ioctlsocket(*instance->login_socket, FIONBIO, &blockingmode);

    CS_READY_PACKET packet;
    packet.size = sizeof CS_READY_PACKET;
    packet.type = CS_READY;

    send(*instance->login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

    char recv_buf[BUFSIZ];

    recv(*instance->login_socket, reinterpret_cast<char*>(recv_buf), BUFSIZ, 0);

    u_long nonblockingmode = 1;
    ioctlsocket(*instance->login_socket, FIONBIO, &nonblockingmode);

    switch (recv_buf[1]) {
    case SC_READY: {
        SC_READY_PACKET* p = reinterpret_cast<SC_READY_PACKET*>(recv_buf);
        OtherReadyPlayerNickName = p->nickname;
        if (strcmp(TCHAR_TO_ANSI(*NICKNAME), TCHAR_TO_ANSI(*OtherReadyPlayerNickName)) == 0) {
            READY = p->ready;
        }
        else {
            OtherNICKNAME = p->nickname;
        }
    }
                 break;
    }
}


