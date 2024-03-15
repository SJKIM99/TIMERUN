// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "GameFramework/CharacterMovementComponent.h"

ATIMERUNController::ATIMERUNController()
{
}

ATIMERUNController::~ATIMERUNController()
{
}

void ATIMERUNController::BeginPlay()
{
	Super::BeginPlay();

	instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

	instance->GetSocketMgr()->ConnectLoginServer();

	login_socket = instance->GetSocketMgr()->GetLoginSocket();

	CS_LOGIN_PACKET packet;
	packet.size = sizeof CS_LOGIN_PACKET;
	packet.type = CS_LOGIN;
	strcpy_s(packet.id, "sungjun426");
	strcpy_s(packet.passwd, "wkd5306");

	int ret = send(*login_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void ATIMERUNController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsActiveIngameSocket)
		RecvPacketFromLoginServer();
	else
		RecvPacketFromIngameServer();

	if (IsEnterNewPlayer)
		UpdateNewPlayer(other_id);
}

void ATIMERUNController::RecvPacketFromLoginServer()
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

void ATIMERUNController::RecvPacketFromIngameServer()
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

void ATIMERUNController::ProcessPakcet(char* packet)
{
    switch (packet[1]) {
    case SC_LOGIN_SUCCESS: {
        SC_LOGIN_SUCCESS_PACKET* p = reinterpret_cast<SC_LOGIN_SUCCESS_PACKET*>(packet);

        auto myplayer = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

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

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        MyPlayerCharacter->id = my_id;

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

        //MyPlayerCharacter->AddMovementInput(characterVelocity);
        MyPlayerCharacter->SetActorLocation(characterLocation);
        MyPlayerCharacter->SetActorRotation(characterRotation);

        GetWorldTimerManager().SetTimer(SendPlayerInfoHandle, this, &ATIMERUNController::SendPlayerupdatePakcet, 0.008f, true);
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
        UCharacterMovementComponent* CharacterMovement = OtherPlayer->GetCharacterMovement();
        FVector NewVelocity = FVector(p->velocity.x, p->velocity.y, OtherPlayer->GetVelocity().Z);
        CharacterMovement->Velocity = NewVelocity;

		OtherPlayer->AddMovementInput(CharacterVelocity);
		OtherPlayer->SetActorLocation(CharacterLocation);
		OtherPlayer->SetActorRotation(CharacterRotation);

        OtherPlayer->HaveGravityGun = p->HaveGravityGun;

        //UE_LOG(LogTemp, Warning, TEXT("%f"), vec_size);
	}
						break;
    case SC_GRAVITYBOX_ADD: {
        SC_GRAVITYBOX_ADD_PACKET* p = reinterpret_cast<SC_GRAVITYBOX_ADD_PACKET*>(packet);

        FVector GravityBoxLocation;

        GravityBoxLocation.X = p->location.x;
        GravityBoxLocation.Y = p->location.y;
        GravityBoxLocation.Z = p->location.z;
        UE_LOG(LogTemp, Warning, TEXT("%f,%f,%f"), p->location.x, p->location.y, p->location.z);
        FRotator GravityBoxRotation;

        GravityBoxRotation.Yaw = p->rotation.x;
        GravityBoxRotation.Pitch = p->rotation.y;
        GravityBoxRotation.Roll = p->rotation.z;

        UpdateNewGravityBox(GravityBoxLocation, GravityBoxRotation, p->box_count);
    }
                             break;
    case SC_GRAVITYBOX_UPDATE: {
        SC_GRAVITYBOX_UPDATE_PACKET* p = reinterpret_cast<SC_GRAVITYBOX_UPDATE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

        FVector GravityBoxLocation;

        GravityBoxLocation.X = p->location.x;
        GravityBoxLocation.Y = p->location.y;
        GravityBoxLocation.Z = p->location.z;
        UE_LOG(LogTemp, Warning, TEXT("%f,%f,%f"), p->location.x, p->location.y, p->location.z);
        FRotator GravityBoxRotation;

        GravityBoxRotation.Yaw = p->rotation.x;
        GravityBoxRotation.Pitch = p->rotation.y;
        GravityBoxRotation.Roll = p->rotation.z;

        AGravityBox* OtherGravityBox = Cast<AGravityBox>(spawnedGravityBox[p->box_count]);

        OtherGravityBox->SetActorLocation(GravityBoxLocation);
        OtherGravityBox->SetActorRotation(GravityBoxRotation);
    }
                             break;
    }
}

void ATIMERUNController::SendPlayerupdatePakcet()
{
    APawn* ControlledPawn = GetPawn();
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

    //UE_LOG(LogTemp, Warning, TEXT("location x y z %f %f %f"), packet.location.x, packet.location.y, packet.location.z);

    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}


void ATIMERUNController::UpdateNewPlayer(int c_id)
{
    UWorld* const world = GetWorld();

    ATIMERUNCharacter* SpawnCharacter = world->SpawnActor<ATIMERUNCharacter>();

    SpawnCharacter->SpawnDefaultController();
    SpawnCharacter->id = c_id;

    UE_LOG(LogTemp, Warning, TEXT("Enter NewPlayer %d"), c_id);
    IsEnterNewPlayer = false;
}

void ATIMERUNController::SortPlayerIndex()
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

void ATIMERUNController::UpdateNewGravityBox(FVector location, FRotator rotation, int box_id)
{
    UWorld* const world = GetWorld();
    AGravityBox* SpawnGravityBox = world->SpawnActor<AGravityBox>(location, rotation);
    SpawnGravityBox->box_count = box_id;
}
