// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

UTIMERUNGameInstance::UTIMERUNGameInstance() : nGravityBox(0)
{
}

void UTIMERUNGameInstance::RecvPacketFromLoginServer()
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

void UTIMERUNGameInstance::RecvPacketFromIngameServer()
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

void UTIMERUNGameInstance::ProcessPakcet(char* packet)
{
    switch (packet[1]) {
        /*case SC_LOGIN_SUCCESS: {
           SC_LOGIN_SUCCESS_PACKET* p = reinterpret_cast<SC_LOGIN_SUCCESS_PACKET*>(packet);

           auto myplayer = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

           memcpy(myplayer->nickname, p->nickname, sizeof p->nickname);

           socketmgr.ConnectIngameServer();
           ingame_socket = socketmgr.GetIngameSocket();

           CS_INGAME_LOGIN_PACKET packet;
           packet.size = sizeof CS_INGAME_LOGIN_PACKET;
           packet.type = CS_INGAME_LOGIN;

           int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

           IsActiveIngameSocket = true;
        }
                        break;*/
    case SC_INGAME_SUCCESS: {

        SC_INGAME_SUCCESS_PACKET* p = reinterpret_cast<SC_INGAME_SUCCESS_PACKET*>(packet);
        my_id = p->id;

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        MyPlayerCharacter->id = my_id;

        UE_LOG(LogTemp, Warning, TEXT("My Id : %d"), my_id);

        MyPlayerCharacter->isChaser = p->ischaser;

        if (MyPlayerCharacter->isChaser) MyPlayerCharacter->TimeChangeCoolTime = CHASER_TIME_CHANGE_COOLTIME;
        else MyPlayerCharacter->TimeChangeCoolTime = RUNNER_TIME_CHANGE_COOLTIME;

        FVector characterVelocity;

        characterVelocity.X = p->velocity.x;
        characterVelocity.Y = p->velocity.y;
        characterVelocity.Z = p->velocity.z;

        FVector characterLocation;

        characterLocation.X = p->location.x;
        characterLocation.Y = p->location.y;
        characterLocation.Z = p->location.z;

        FRotator characterRotation;

        characterRotation.Yaw = p->yaw;
        characterRotation.Pitch = 0;
        characterRotation.Roll = 0;

        MyPlayerCharacter->nickname = FString(ANSI_TO_TCHAR(p->nickname));

        MyPlayerCharacter->MyScore = p->score;

        MyPlayerCharacter->CanTimeChange = p->CanTimeChange;

        MyPlayerCharacter->CanTakePicture = p->CanTakePicture;

        MyPlayerCharacter->CanSpawnGravityBox = p->CanSpawnGravityBox;

        //MyPlayerCharacter->AddMovementInput(characterVelocity);
        MyPlayerCharacter->SetActorLocation(characterLocation);
        MyPlayerCharacter->SetActorRotation(characterRotation);

        GetWorld()->GetTimerManager().SetTimer(SendPlayerInfoHandle, this, &UTIMERUNGameInstance::SendPlayerupdatePakcet, 0.1f, true);
    }
                          break;

    case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
        IsEnterNewPlayer = true;
        other_id = p->id;

        FVector OtherPlayerLocation;
        OtherPlayerLocation.X = p->location.x;
        OtherPlayerLocation.Y = p->location.y;
        OtherPlayerLocation.Z = p->location.z;

        UpdateNewPlayer(other_id, OtherPlayerLocation, FString(ANSI_TO_TCHAR(p->nickname)), p->time, p->ischaser);
    }
                      break;
    case SC_LOGIN_FAIL: {

    }
                      break;
    case SC_SIGNUP: {
        SC_SIGNUP_PACKET* p = reinterpret_cast<SC_SIGNUP_PACKET*>(packet);
    }
                  break;
    case SC_WORLD_UPDATE: {
        SC_WORLD_UPDATE_PACKET* p = reinterpret_cast<SC_WORLD_UPDATE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        //위 함수는 spawnCharacters에 world에 생성된 객체를 넣지만, 월드에 스폰된 순서로 들어가기에 아이디를 사용한 오름차순 정렬이 필요하다.
        SortPlayerIndex();

        FVector CharacterVelocity;
        CharacterVelocity.X = p->velocity.x;
        CharacterVelocity.Y = p->velocity.y;
        CharacterVelocity.Z = p->velocity.z;

        FVector CharacterLocation;
        CharacterLocation.X = p->location.x;
        CharacterLocation.Y = p->location.y;
        CharacterLocation.Z = p->location.z;

        FRotator CharacterRotation;
        CharacterRotation.Yaw = p->yaw;
        CharacterRotation.Pitch = 0;
        CharacterRotation.Roll = 0;

        ATIMERUNCharacter* OtherPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);
        /* UCharacterMovementComponent* CharacterMovement = OtherPlayer->GetCharacterMovement();
         FVector NewVelocity = FVector(p->velocity.x, p->velocity.y, OtherPlayer->GetVelocity().Z);
         CharacterMovement->Velocity = NewVelocity;
         OtherPlayer->AddMovementInput(CharacterVelocity);*/

        UpdatePosition(CharacterLocation, CharacterRotation, CharacterVelocity, p->id);
        //OtherPlayer->SetActorLocation(CharacterLocation);
        //OtherPlayer->SetActorRotation(CharacterRotation);

        OtherPlayer->HaveGravityGun = p->HaveGravityGun;
        OtherPlayer->isLanded = p->isLanded;
        OtherPlayer->HaveTimeMachine = p->HaveTimeMachine;
        OtherPlayer->DoingTimeTravel = p->DoingTimeTravel;

        //UE_LOG(LogTemp, Warning, TEXT("%f"), vec_size);
    }
                        break;
    case SC_GRAVITYBOX_ADD: {
        SC_GRAVITYBOX_ADD_PACKET* p = reinterpret_cast<SC_GRAVITYBOX_ADD_PACKET*>(packet);
        if (p->id == my_id) {
            //서버로부터 받은 box의 아이디만 설정해준다 
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

            AGravityBox* MyGravityBox = Cast<AGravityBox>(spawnedGravityBox[p->boxid]);
            MyGravityBox->BoxId = p->boxid;
            MyGravityBox->box_time = p->box_time;
            UE_LOG(LogTemp, Warning, TEXT("My GravityBox Spawned %d"), p->boxid);
        }
        else {
            FVector GravityBoxLocation;
            GravityBoxLocation.X = p->location.x;
            GravityBoxLocation.Y = p->location.y;
            GravityBoxLocation.Z = p->location.z;

            FRotator GravityBoxRotation;
            GravityBoxRotation.Yaw = p->rotation.x;
            GravityBoxRotation.Pitch = p->rotation.y;
            GravityBoxRotation.Roll = p->rotation.z;
            UpdateNewGravityBox(GravityBoxLocation, GravityBoxRotation, p->boxid, p->id, p->box_time);
            UE_LOG(LogTemp, Warning, TEXT("Other GravityBox Spawned %d"), p->boxid);
        }
    }
                          break;
    case SC_GRAVITYBOX_UPDATE: {
        SC_GRAVITYBOX_UPDATE_PACKET* p = reinterpret_cast<SC_GRAVITYBOX_UPDATE_PACKET*>(packet);

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();

        ATIMERUNCharacter* OtherPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);

        if (MyPlayerCharacter->my_time == OtherPlayer->my_time) {
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);
            FVector GravityBoxLocation;
            GravityBoxLocation.X = p->location.x;
            GravityBoxLocation.Y = p->location.y;
            GravityBoxLocation.Z = p->location.z;

            FRotator GravityBoxRotation;
            GravityBoxRotation.Yaw = p->rotation.x;
            GravityBoxRotation.Pitch = p->rotation.y;
            GravityBoxRotation.Roll = p->rotation.z;

            FVector GravityBoxVelocity;
            GravityBoxVelocity.X = p->velocity.x;
            GravityBoxVelocity.Y = p->velocity.y;
            GravityBoxVelocity.Z = p->velocity.z;

            AGravityBox* OtherGravityBox = Cast<AGravityBox>(spawnedGravityBox[p->boxid]);

            OtherGravityBox->ByWhoID = p->id;
            OtherGravityBox->isGrabbed = p->isGrabbed;
            OtherGravityBox->box_time = p->time;
            OtherGravityBox->IsMoving = p->ismoving;
            OtherGravityBox->CanFixPos = p->Canfixpos;
            OtherGravityBox->CanFall = p->canfall;

            UpdateGravityBoxPosition(GravityBoxLocation, GravityBoxRotation, GravityBoxVelocity, p->boxid);
        }
    }
                             break;
    case SC_PLAYER_JUMP: {
        SC_PLAYER_JUMP_PACKET* p = reinterpret_cast<SC_PLAYER_JUMP_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();

        ATIMERUNCharacter* JumpPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);
        JumpPlayer->Jump();
        UE_LOG(LogTemp, Warning, TEXT("JumpPlayer->isLanded %d"), JumpPlayer->isLanded);
    }
                       break;
    case SC_GRAVIRTBOX_GRABBED: {
        SC_GRAVIRTBOX_GRABBED_PACKET* p = reinterpret_cast<SC_GRAVIRTBOX_GRABBED_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);
        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();
        ATIMERUNCharacter* OtherPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);

        if (MyPlayerCharacter->my_time == OtherPlayer->my_time) {
            AGravityBox* GravityBox = Cast<AGravityBox>(spawnedGravityBox[p->box_id]);
            GravityBox->ByWhoID = p->id;
            GravityBox->isGrabbed = p->isGrabbed;
        }
    }
                              break;
    case SC_GRAVIRTBOX_DROPPED: {
        SC_GRAVIRTBOX_DROPPED_PACKET* p = reinterpret_cast<SC_GRAVIRTBOX_DROPPED_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);
        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();
        ATIMERUNCharacter* OtherPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);
        if (MyPlayerCharacter->my_time == OtherPlayer->my_time) {
            AGravityBox* GravityBox = Cast<AGravityBox>(spawnedGravityBox[p->box_id]);
            GravityBox->ByWhoID = p->id;
            GravityBox->isGrabbed = p->isGrabbed;
        }
    }
                              break;
    case SC_TIME_CHANGE: {
        SC_TIME_CHANGE_PACKET* p = reinterpret_cast<SC_TIME_CHANGE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

        ATIMERUNCharacter* TimeChangePlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);

        TimeChangePlayer->my_time = p->time;
        //이 작업은 월드에 있는 모든 클라이언트가 실행해야해
        //p->id는 시간이동을 한 클라이언트야
        if (TimeChangePlayer->id == my_id) {
            for (int i = 0; i < spawnedGravityBox.Num(); ++i) {
                AGravityBox* GravityBox = Cast<AGravityBox>(spawnedGravityBox[i]);
                FVector ThisTimeLocation;
                ThisTimeLocation.X = GravityBox->timestate_location[p->time].X;
                ThisTimeLocation.Y = GravityBox->timestate_location[p->time].Y;
                ThisTimeLocation.Z = GravityBox->timestate_location[p->time].Z;

                FRotator ThisTimeRotation;
                ThisTimeRotation.Yaw = GravityBox->timestate_rotation[TimeChangePlayer->my_time].Yaw;
                ThisTimeRotation.Pitch = GravityBox->timestate_rotation[TimeChangePlayer->my_time].Pitch;
                ThisTimeRotation.Roll = GravityBox->timestate_rotation[TimeChangePlayer->my_time].Roll;

                GravityBox->SetActorLocation(ThisTimeLocation);
                GravityBox->SetActorRotation(ThisTimeRotation);

            }
        }
    }
                       break;
    case SC_GAME_START: {
        SC_GAME_START_PACKET* p = reinterpret_cast<SC_GAME_START_PACKET*>(packet);

        socketmgr.ConnectIngameServer();
        ingame_socket = socketmgr.GetIngameSocket();

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        MyPlayerCharacter->GameStartCountDown = false;
        // MyPlayerCharacter->isChaser = p->ischaser;

        CS_INGAME_LOGIN_PACKET login_packet;
        login_packet.size = sizeof CS_INGAME_LOGIN_PACKET;
        login_packet.type = CS_INGAME_LOGIN;
        strcpy_s(login_packet.nickname, TCHAR_TO_ANSI(*MyPlayerCharacter->nickname));
        login_packet.location.x = -240;
        login_packet.location.y = -1410 + rand() % 200;
        login_packet.location.z = 230;
        login_packet.my_time = MyPlayerCharacter->my_time;

        int ret = send(*ingame_socket, reinterpret_cast<char*>(&login_packet), sizeof(login_packet), 0);

       // IsActiveIngameSocket = true;

        GameStart = true;
    }
                      break;
    case SC_GRAVITYBOX_TIME_STATE: {
        UE_LOG(LogTemp, Warning, TEXT("SC_GRAVITYBOX_TIME_STATE"));

        SC_GRAVITYBOX_TIME_STATE_PACKET* p = reinterpret_cast<SC_GRAVITYBOX_TIME_STATE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

        AGravityBox* GravityBox = Cast<AGravityBox>(spawnedGravityBox[p->boxid]);

        FVector ThisTimeLocation;
        FRotator ThisTimeRotation;

        ThisTimeLocation.X = p->location.x;
        ThisTimeLocation.Y = p->location.y;
        ThisTimeLocation.Z = p->location.z;

        ThisTimeRotation.Yaw = p->rotation.x;
        ThisTimeRotation.Pitch = p->rotation.y;
        ThisTimeRotation.Roll = p->rotation.z;

        for (int i = p->timestate; i < TIMESIZE; ++i) {
            GravityBox->timestate_location[i].X = p->location.x;
            GravityBox->timestate_location[i].Y = p->location.y;
            GravityBox->timestate_location[i].Z = p->location.z;

            GravityBox->timestate_rotation[i].Yaw = p->rotation.x;
            GravityBox->timestate_rotation[i].Pitch = p->rotation.y;
            GravityBox->timestate_rotation[i].Roll = p->rotation.z;

        }

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        if (MyPlayerCharacter->my_time >= p->timestate) {
            GravityBox->SetActorLocation(ThisTimeLocation);
            GravityBox->SetActorRotation(ThisTimeRotation);
        }
    }
                                 break;
    case SC_GAME_TIMER: {
        SC_GAME_TIMER_ON_PACKET* p = reinterpret_cast<SC_GAME_TIMER_ON_PACKET*>(packet);
        seconds = p->seconds;
    }
                      break;
    case SC_TEAM_CHANGE: {
        SC_TEAM_CHANGE_PACKET* p = reinterpret_cast<SC_TEAM_CHANGE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();

        ATIMERUNCharacter* Myplayer = Cast<ATIMERUNCharacter>(spawnedCharacters[my_id]);
        Myplayer->isChaser = p->ischaser;

        if (Myplayer->TimeChangeCoolTime == CHASER_TIME_CHANGE_COOLTIME) Myplayer->TimeChangeCoolTime = RUNNER_TIME_CHANGE_COOLTIME;
        else  Myplayer->TimeChangeCoolTime = CHASER_TIME_CHANGE_COOLTIME;



        ATIMERUNCharacter* OtherPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[other_id]);
        OtherPlayer->isChaser = !p->ischaser;

    }
                       break;
    case SC_CALCULATE_SCORE: {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        SortPlayerIndex();

        SC_CALCULATE_SCORE_PACKET* p = reinterpret_cast<SC_CALCULATE_SCORE_PACKET*>(packet);

        ATIMERUNCharacter* OtherPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[p->id]);

        OtherPlayer->MyScore = p->score;
    }
                           break;
    case SC_CAN_TAKE_PICTURE: {
        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        SC_CAN_TAKE_PICTURE_PACKET* p = reinterpret_cast<SC_CAN_TAKE_PICTURE_PACKET*>(packet);

        MyPlayerCharacter->CanTakePicture = p->cantakepicture;
    }
                            break;
    case SC_CAN_SPAWN_GRAVITYBOX: {
        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        SC_CAN_SPAWN_GRAVITYBOX_PACKET* p = reinterpret_cast<SC_CAN_SPAWN_GRAVITYBOX_PACKET*>(packet);

        MyPlayerCharacter->CanSpawnGravityBox = p->canspawngravitybox;
    }
                                break;
    case SC_ALL_PLAYER_READY: {
        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        MyPlayerCharacter->GameStartCountDown = true;
    }
                            break;
    case SC_CAN_TIME_CHANGE: {
        SC_CAN_TIME_CHANGE_PACKET* p = reinterpret_cast<SC_CAN_TIME_CHANGE_PACKET*>(packet);

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        MyPlayerCharacter->CanTimeChange = p->cantimechange;

    }
                           break;
    case SC_GAME_END: {
        UE_LOG(LogTemp, Warning, TEXT("SC_GAME_END"));

        GameStart = false;

        GetWorld()->GetTimerManager().SetTimer(GameEndHandle, this, &UTIMERUNGameInstance::GameEnd, 3.0f, false);
    }
                    break;
    }
}


void UTIMERUNGameInstance::SendPlayerupdatePakcet()
{
    if (!GameStart) return;

    APlayerController* PlayerContoller = GetWorld()->GetFirstPlayerController();
    APawn* ControlledPawn = PlayerContoller->GetPawn();
    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(ControlledPawn);

    CS_PLAYER_UPDATE_PACKET packet;
    packet.size = sizeof CS_PLAYER_UPDATE_PACKET;
    packet.type = CS_PLAYER_UPDATE;
    packet.id = my_id;
    packet.location.x = MyPlayerCharacter->GetActorLocation().X;
    packet.location.y = MyPlayerCharacter->GetActorLocation().Y;
    packet.location.z = MyPlayerCharacter->GetActorLocation().Z;
    packet.velocity.x = MyPlayerCharacter->GetVelocity().X;
    packet.velocity.y = MyPlayerCharacter->GetVelocity().Y;
    packet.velocity.z = MyPlayerCharacter->GetVelocity().Z;
    packet.yaw = MyPlayerCharacter->GetActorRotation().Yaw;
    packet.HaveGravityGun = MyPlayerCharacter->HaveGravityGun;
    packet.time = MyPlayerCharacter->my_time;
    packet.isLanded = MyPlayerCharacter->isLanded;
    packet.HaveTimeMachine = MyPlayerCharacter->HaveTimeMachine;
    packet.DoingTimeTravel = MyPlayerCharacter->DoingTimeTravel;

    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}

void UTIMERUNGameInstance::UpdateNewPlayer(int c_id, FVector location, FString nickname, int time, bool role)
{
    UWorld* const world = GetWorld();

    ATIMERUNCharacter* SpawnCharacter = world->SpawnActor<ATIMERUNCharacter>(ATIMERUNCharacter::StaticClass(), location, FRotator::ZeroRotator);

    SpawnCharacter->SpawnDefaultController();
    SpawnCharacter->id = c_id;
    SpawnCharacter->nickname = nickname;
    SpawnCharacter->my_time = time;
    SpawnCharacter->isChaser = role;


    IsEnterNewPlayer = false;
}

void UTIMERUNGameInstance::SortPlayerIndex()
{
    auto CompareByPlayerId = [](const AActor& A, const AActor& B) {
        const ATIMERUNCharacter* CharacterA = Cast<ATIMERUNCharacter>(&A);
        const ATIMERUNCharacter* CharacterB = Cast<ATIMERUNCharacter>(&B);
        if (CharacterA && CharacterB) {
            return CharacterA->id < CharacterB->id;
        }
        // A 또는 B가 ATIMERUNCharacterVer2가 아닌 경우에는 정렬하지 않음
        return false;
        };
    spawnedCharacters.Sort(CompareByPlayerId);
}

void UTIMERUNGameInstance::SendGravityBoxSpawn(FVector location, FRotator rotation)
{
    if (!GameStart) return;

    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    CS_GRAVITYBOX_ADD_PACKET packet;
    packet.id = MyPlayerCharacter->id;
    packet.type = CS_GRAVITYBOX_ADD;
    packet.size = sizeof CS_GRAVITYBOX_ADD_PACKET;
    packet.location.x = location.X;
    packet.location.y = location.Y;
    packet.location.z = location.Z;
    packet.rotation.x = rotation.Yaw;
    packet.rotation.y = rotation.Pitch;
    packet.rotation.z = rotation.Roll;
    //중력박스 time은 스폰한 캐릭터의 time의 좌우된다.
    packet.time = MyPlayerCharacter->my_time;

    if (ingame_socket == NULL) return;
    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

    UE_LOG(LogTemp, Warning, TEXT("GravityBox Spawned"));
}


void UTIMERUNGameInstance::UpdateNewGravityBox(FVector location, FRotator rotation, int box_id, int bywhoid, int box_time)
{
    UWorld* const world = GetWorld();
    AGravityBox* SpawnGravityBox = world->SpawnActor<AGravityBox>(location, rotation);
    if (!SpawnGravityBox)
    {
        FVector newLocation;
        newLocation.X = location.X;
        newLocation.Y = location.Y;
        newLocation.Z = location.Z - 1000;

        SpawnGravityBox = world->SpawnActor<AGravityBox>(newLocation, rotation);
    }
    SpawnGravityBox->BoxId = box_id;
    SpawnGravityBox->ByWhoID = bywhoid;
    SpawnGravityBox->box_time = box_time;
}


void UTIMERUNGameInstance::UpdatePosition(FVector new_location, FRotator new_rotation, FVector new_velocity, int player_id)
{
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
    SortPlayerIndex();

    ATIMERUNCharacter* RecvUpdatePacketPlayer = Cast<ATIMERUNCharacter>(spawnedCharacters[player_id]);

    RecvUpdatePacketPlayer->prev_location = RecvUpdatePacketPlayer->current_location;
    RecvUpdatePacketPlayer->prev_rotation = RecvUpdatePacketPlayer->current_rotation;
    RecvUpdatePacketPlayer->prev_velocity = RecvUpdatePacketPlayer->current_velocity;

    RecvUpdatePacketPlayer->current_location = new_location;
    RecvUpdatePacketPlayer->current_rotation = new_rotation;
    RecvUpdatePacketPlayer->current_velocity = new_velocity;

    FTimerDelegate TimerCallback;
    TimerCallback.BindLambda([=, this]() {
        InterpolatePosition(RecvUpdatePacketPlayer);
        });

    GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, TimerCallback, GetWorld()->GetDeltaSeconds(), true);
}

void UTIMERUNGameInstance::InterpolatePosition(ATIMERUNCharacter* UpdatePlayer)
{
    float InterpSpeed = 5;
    //float DeltaSeconds = 0.008;
    float DeltaSeconds = GetWorld()->GetDeltaSeconds();



    FVector NewLocation = FMath::VInterpTo(UpdatePlayer->GetActorLocation(), UpdatePlayer->current_location, DeltaSeconds, InterpSpeed);
    FRotator NewRotation = FMath::RInterpTo(UpdatePlayer->GetActorRotation(), UpdatePlayer->current_rotation, DeltaSeconds, InterpSpeed);
    FVector NewVelocity = UpdatePlayer->current_velocity;
    //FVector NewVelocity = FMath::VInterpTo(UpdatePlayer->GetVelocity(), UpdatePlayer->current_velocity, DeltaSeconds, InterpSpeed);


    UCharacterMovementComponent* CharacterMovement = UpdatePlayer->GetCharacterMovement();
    CharacterMovement->Velocity = NewVelocity;

    UpdatePlayer->AddMovementInput(NewVelocity);
    UpdatePlayer->SetActorLocation(NewLocation);
    UpdatePlayer->SetActorRotation(NewRotation);
}

void UTIMERUNGameInstance::UpdateGravityBoxPosition(FVector new_location, FRotator new_rotation, FVector new_velocity, int box_id)
{
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

    AGravityBox* RecvUpdatePacketGravityBox = Cast<AGravityBox>(spawnedGravityBox[box_id]);

    RecvUpdatePacketGravityBox->current_location = new_location;
    RecvUpdatePacketGravityBox->current_rotation = new_rotation;
    RecvUpdatePacketGravityBox->current_velocity = new_velocity;

    RecvUpdatePacketGravityBox->TimerCallback.BindLambda([=, this]() {
        InterporlateGravityBoxPosition(RecvUpdatePacketGravityBox);
        });

    GetWorld()->GetTimerManager().SetTimer(RecvUpdatePacketGravityBox->MoveGravityBoxTimerHandle, RecvUpdatePacketGravityBox->TimerCallback, GetWorld()->GetDeltaSeconds(), true);
}

void UTIMERUNGameInstance::InterporlateGravityBoxPosition(AGravityBox* UpdateGravityBox)
{
    if (UpdateGravityBox->CanFixPos == false && my_id != UpdateGravityBox->ByWhoID) {
        float InterpSpeed = 5;
        //float DeltaSeconds = 0.008;
        float DeltaSeconds = GetWorld()->GetDeltaSeconds();


        FVector NewLocation = FMath::VInterpTo(UpdateGravityBox->GetActorLocation(), UpdateGravityBox->current_location, DeltaSeconds, InterpSpeed);
        FRotator NewRotation = FMath::RInterpTo(UpdateGravityBox->GetActorRotation(), UpdateGravityBox->current_rotation, DeltaSeconds, InterpSpeed);
        FVector NewVelocity = UpdateGravityBox->current_velocity;

        UpdateGravityBox->AddMovementInput(NewVelocity);
        UpdateGravityBox->SetActorLocation(NewLocation);
        UpdateGravityBox->SetActorRotation(NewRotation);
    }

}

void UTIMERUNGameInstance::InitLoginSocket()
{
    if (socketmgr.ConnectLoginServer()) {
        UE_LOG(LogTemp, Warning, TEXT("Success the login server connect"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Fail the login server connect"));
    }
}

void UTIMERUNGameInstance::InitIngameSocket()
{
    if (socketmgr.ConnectIngameServer()) {
        UE_LOG(LogTemp, Warning, TEXT("Success the Ingame server connect"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Fail the Ingame server connect"));
    }
}

void UTIMERUNGameInstance::SendPlayerJumpPacket()
{
    if (!GameStart) return;

    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    CS_PLAYER_JUMP_PACKET packet;
    packet.size = sizeof CS_PLAYER_JUMP_PACKET;
    packet.type = CS_PLAYER_JUMP;
    packet.id = my_id;

    if (ingame_socket == NULL) return;
    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void UTIMERUNGameInstance::SendPlayerLandedPacket()
{
}

void UTIMERUNGameInstance::SendTimeChangePacket()
{
    if (!GameStart) return;

    APlayerController* PlayerContoller = GetWorld()->GetFirstPlayerController();
    APawn* ControlledPawn = PlayerContoller->GetPawn();
    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(ControlledPawn);

    CS_TIME_CHANGE_PACKET packet;
    packet.size = sizeof CS_TIME_CHANGE_PACKET;
    packet.type = CS_TIME_CHANGE;
    packet.id = my_id;
    packet.time = MyPlayerCharacter->my_time;

    if (ingame_socket == NULL) return;
    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void UTIMERUNGameInstance::SendCameraScorePacket()
{
    if (!GameStart) return;

    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    CS_TAKE_PICTURE_PACKET packet;
    packet.size = sizeof CS_TAKE_PICTURE_PACKET;
    packet.type = CS_TAKE_PICTURE;
    packet.score = MyPlayerCharacter->MyScore;

    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void UTIMERUNGameInstance::GameEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(GameEndHandle);

    GetWorld()->GetTimerManager().ClearTimer(SendPlayerInfoHandle);
    GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
    SortPlayerIndex();

    for (auto& character : spawnedCharacters) {
        ATIMERUNCharacter* player = Cast<ATIMERUNCharacter>(character);
        if (player != MyPlayerCharacter) {
            player->Destroy();
        }
    }

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

    for (auto& box : spawnedGravityBox) {
        AGravityBox* g_box = Cast<AGravityBox>(box);
        g_box->GetWorld()->GetTimerManager().ClearTimer(g_box->SendGravityBoxInfoHandle);
        g_box->GetWorld()->GetTimerManager().ClearTimer(g_box->MoveGravityBoxTimerHandle);
        g_box->Destroy();

    }
}
