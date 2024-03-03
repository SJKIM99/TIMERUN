// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNController_Ver2.h"
#include "Kismet/GameplayStatics.h"

ATIMERUNController_Ver2::ATIMERUNController_Ver2()
{
}

ATIMERUNController_Ver2::~ATIMERUNController_Ver2()
{
}

void ATIMERUNController_Ver2::BeginPlay()
{
	Super::BeginPlay();

	instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

	instance->GetSocketMgr()->ConnectLoginServer();

	login_socket = instance->GetSocketMgr()->GetLoginSocket();

	CS_LOGIN_PACKET packet;
	packet.size = sizeof CS_LOGIN_PACKET;
	packet.type = CS_LOGIN;
	strcpy_s(packet.id, "sungjun4264");
	strcpy_s(packet.passwd, "wkd5306s");

	int ret = send(*login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void ATIMERUNController_Ver2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsActiveIngameSocket)
		RecvPacketFromLoginServer();
	else
		RecvPacketFromIngameServer();

    if (isPlayermove) {
        APawn* ControlledPawn = GetPawn();
        SendMovePacket(ControlledPawn);
    }
	if (IsEnterNewPlayer)
		UpdateNewPlayer(other_id);
}

void ATIMERUNController_Ver2::RecvPacketFromLoginServer()
{
    char buf[BUF_SIZE];
    int ret = recv(*login_socket, reinterpret_cast<char*>(&buf), BUF_SIZE, 0);
    if (ret <= 0) {
        return;
    }
    else if (ret > BUF_SIZE - login_prev_remain_data) {
        UE_LOG(LogTemp, Warning, TEXT("overflow data, recv data size: %d"), ret);
        login_prev_remain_data = 0;
        return;
    }

    if (login_prev_remain_data > 0) {
        UE_LOG(LogTemp, Warning, TEXT("exist prev remain data"));
        memcpy(login_prev_packet_buf + login_prev_remain_data, buf, ret);
    }
    else {
        memcpy(login_prev_packet_buf, buf, ret);
    }

    int remain_data = ret + login_prev_remain_data;
    char* p = login_prev_packet_buf;
    while (remain_data > 0) {
        int packet_size = p[0];
        if (packet_size == 0) {
            login_prev_remain_data = 0;
            return;
        }
        if (packet_size <= remain_data) {
            ProcessPakcet(p);
            p = p + packet_size;
            remain_data -= packet_size;
        }
        else break;
    }
    login_prev_remain_data = remain_data;
    if (remain_data > 0) {
        memcpy(login_prev_packet_buf, p, remain_data);
    }
}

void ATIMERUNController_Ver2::RecvPacketFromIngameServer()
{
    char buf[BUF_SIZE];
    int ret = recv(*ingame_socket, reinterpret_cast<char*>(&buf), BUF_SIZE, 0);
    if (ret <= 0) {
        return;
    }
    else if (ret > BUF_SIZE - ingame_prev_remain_data) {
        UE_LOG(LogTemp, Warning, TEXT("overflow data, recv data size: %d"), ret);
        ingame_prev_remain_data = 0;
        return;
    }

    if (ingame_prev_remain_data > 0) {
        UE_LOG(LogTemp, Warning, TEXT("exist prev remain data"));
        memcpy(ingame_prev_packet_buf + ingame_prev_remain_data, buf, ret);
    }
    else {
        memcpy(ingame_prev_packet_buf, buf, ret);
    }

    int remain_data = ret + ingame_prev_remain_data;
    char* p = ingame_prev_packet_buf;
    while (remain_data > 0) {
        int packet_size = p[0];
        if (packet_size == 0) {
            ingame_prev_remain_data = 0;
            return;
        }
        if (packet_size <= remain_data) {
            ProcessPakcet(p);
            p = p + packet_size;
            remain_data -= packet_size;
        }
        else break;
    }
    ingame_prev_remain_data = remain_data;
    if (remain_data > 0) {
        memcpy(ingame_prev_packet_buf, p, remain_data);
    }
}

void ATIMERUNController_Ver2::ProcessPakcet(char* packet)
{
    switch (packet[1]) {
    case SC_LOGIN_SUCCESS: {
        SC_LOGIN_SUCCESS_PACKET* p = reinterpret_cast<SC_LOGIN_SUCCESS_PACKET*>(packet);

        auto myplayer = Cast<ATIMERUNCharacter_Ver2>(UGameplayStatics::GetPlayerCharacter(this, 0));

        memcpy(myplayer->nickname, p->nickname, sizeof p->nickname);

        instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

        instance->GetSocketMgr()->ConnectIngameServer();

        ingame_socket = instance->GetSocketMgr()->GetIngameSocket();

        CS_INGAME_LOGIN_PACKET packet;
        packet.size = sizeof CS_INGAME_LOGIN_PACKET;
        packet.type = CS_INGAME_LOGIN;

        int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

        IsActiveIngameSocket = true;
    }
                         break;
    case SC_INGAME_SUCCESS: {
        SC_INGAME_SUCCESS_PACKET* p = reinterpret_cast<SC_INGAME_SUCCESS_PACKET*>(packet);
        my_id = p->id;

        ATIMERUNCharacter_Ver2* MyPlayerCharacter = Cast<ATIMERUNCharacter_Ver2>(UGameplayStatics::GetPlayerCharacter(this, 0));

        MyPlayerCharacter->id = my_id;

        FVector myCharacterLocation;

        myCharacterLocation.X = 0;
        myCharacterLocation.Y = 0;
        myCharacterLocation.Z = 0;

        FRotator myCharacterRotation;

        myCharacterRotation.Yaw = 0;
        myCharacterRotation.Pitch = 0;
        myCharacterRotation.Roll = 0;

        auto myplayer = Cast<ATIMERUNCharacter_Ver2>(UGameplayStatics::GetPlayerCharacter(this, 0));

        myplayer->SetActorLocation(myCharacterLocation);
        myplayer->SetActorRotation(myCharacterRotation);
    }
                          break;

    case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
        IsEnterNewPlayer = true;
        other_id = p->id;

        UE_LOG(LogTemp, Warning, TEXT("Other id %d"), other_id);
    }
                      break;
    case SC_LOGIN_FAIL: {

    }
                      break;
    case SC_SIGNUP: {
        SC_SIGNUP_PACKET* p = reinterpret_cast<SC_SIGNUP_PACKET*>(packet);
    }
                  break;
    case SC_MOVE_PLAYER: {
        SC_MOVE_PACKET* p = reinterpret_cast<SC_MOVE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter_Ver2::StaticClass(), spawnedCharacters);
        //위 함수는 spawnCharacters에 world에 생성된 객체를 넣지만, 월드에 스폰된 순서로 들어가기에 아이디를 사용한 오름차순 정렬이 필요하다.
        SortPlayerIndex();

        FVector CharacterLocation;
        CharacterLocation.X = p->location.x;
        CharacterLocation.Y = p->location.y;
        CharacterLocation.Z = p->location.z;

        FRotator CharacterRotation;
        CharacterRotation.Yaw = p->yaw;
        CharacterRotation.Pitch = 0;
        CharacterRotation.Roll = 0;

        if (p->id == my_id) {
            spawnedCharacters[my_id]->SetActorLocation(CharacterLocation);
            spawnedCharacters[my_id]->SetActorRotation(CharacterRotation);
        }
        else {
            spawnedCharacters[p->id]->SetActorLocation(CharacterLocation);
            spawnedCharacters[p->id]->SetActorRotation(CharacterRotation);
        }
    }
                       break;
    }
}

void ATIMERUNController_Ver2::SendMovePacket(/*direction direction,*/ APawn* pawn)
{
    CS_MOVE_PACKET packet;
    packet.size = sizeof CS_MOVE_PACKET;
    packet.type = CS_MOVE;
    packet.id = my_id;
    /*packet.direction = direction;*/
    packet.yaw = pawn->GetActorRotation().Yaw;
    packet.location.x = pawn->GetActorLocation().X;
    packet.location.y = pawn->GetActorLocation().Y;
    packet.location.z = pawn->GetActorLocation().Z;

    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}

void ATIMERUNController_Ver2::UpdateNewPlayer(int c_id)
{
    UWorld* const world = GetWorld();

    ATIMERUNCharacter_Ver2* SpawnCharacter = world->SpawnActor<ATIMERUNCharacter_Ver2>();

    SpawnCharacter->SpawnDefaultController();
    SpawnCharacter->id = c_id;

    UE_LOG(LogTemp, Warning, TEXT("Enter NewPlayer %d"), c_id);
    IsEnterNewPlayer = false;
}

void ATIMERUNController_Ver2::SortPlayerIndex()
{
    auto CompareByPlayerId = [](const AActor& A, const AActor& B) {
        const ATIMERUNCharacter_Ver2* CharacterA = Cast<ATIMERUNCharacter_Ver2>(&A);
        const ATIMERUNCharacter_Ver2* CharacterB = Cast<ATIMERUNCharacter_Ver2>(&B);
        if (CharacterA && CharacterB) {
            return CharacterA->id < CharacterB->id;
        }
        // A 또는 B가 ATIMERUNCharacterVer2가 아닌 경우에는 정렬하지 않음
        return false;
    };
    spawnedCharacters.Sort(CompareByPlayerId);
}
