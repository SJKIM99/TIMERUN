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
    case SC_LOGIN_SUCCESS: {
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

        GetWorld()->GetTimerManager().SetTimer(SendPlayerInfoHandle, this, &UTIMERUNGameInstance::SendPlayerupdatePakcet, 0.032f, true);
    }
                          break;

    case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
        IsEnterNewPlayer = true;
        other_id = p->id;
        UpdateNewPlayer(other_id);
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

        if (p->id == my_id) {
            //서버로부터 받은 box의 아이디만 설정해준다 
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

            AGravityBox* MyGravityBox = Cast<AGravityBox>(spawnedGravityBox[p->boxid]);
            MyGravityBox->BoxId = p->boxid;
            //CheckGravityBoxSpawn(p->boxid);
        }
		else {
            nGravityBox = p->boxid + 1;

			FVector GravityBoxLocation;
			GravityBoxLocation.X = p->location.x;
			GravityBoxLocation.Y = p->location.y;
			GravityBoxLocation.Z = p->location.z;

			FRotator GravityBoxRotation;
			GravityBoxRotation.Yaw = p->rotation.x;
			GravityBoxRotation.Pitch = p->rotation.y;
			GravityBoxRotation.Roll = p->rotation.z;
			UpdateNewGravityBox(GravityBoxLocation, GravityBoxRotation, p->boxid);
		}
    }
                          break;
  /*  case SC_GRAVITYBOX_UPDATE: {
        SC_GRAVITYBOX_UPDATE_PACKET* p = reinterpret_cast<SC_GRAVITYBOX_UPDATE_PACKET*>(packet);

        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

        FVector GravityBoxLocation;
        GravityBoxLocation.X = p->location.x;
        GravityBoxLocation.Y = p->location.y;
        GravityBoxLocation.Z = p->location.z;

        FRotator GravityBoxRotation;
        GravityBoxRotation.Yaw = p->rotation.x;
        GravityBoxRotation.Pitch = p->rotation.y;
        GravityBoxRotation.Roll = p->rotation.z;

        AGravityBox* OtherGravityBox = Cast<AGravityBox>(spawnedGravityBox[p->boxid]);

        OtherGravityBox->SetActorLocation(GravityBoxLocation);
        OtherGravityBox->SetActorRotation(GravityBoxRotation);
    }
                             break;*/
    }
}

void UTIMERUNGameInstance::SendPlayerupdatePakcet()
{
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

    //UE_LOG(LogTemp, Warning, TEXT("location x y z %f %f %f"), packet.location.x, packet.location.y, packet.location.z);

    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}

void UTIMERUNGameInstance::UpdateNewPlayer(int c_id)
{
    UWorld* const world = GetWorld();

    ATIMERUNCharacter* SpawnCharacter = world->SpawnActor<ATIMERUNCharacter>();

    SpawnCharacter->SpawnDefaultController();
    SpawnCharacter->id = c_id;

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

bool UTIMERUNGameInstance::CheckGravityBoxSpawn()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);

    if (spawnedGravityBox.Num() == nGravityBox + 1) {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravityBox::StaticClass(), spawnedGravityBox);
        AGravityBox* SpawnGravityBox = Cast<AGravityBox>(spawnedGravityBox[nGravityBox]); //박스의 위치값, FVector형태로 들어감

        SpawnGravityBox->BoxLocation = SpawnGravityBox->GetActorLocation();
        SpawnGravityBox->BoxRotation = SpawnGravityBox->GetActorRotation();

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        CS_GRAVITYBOX_ADD_PACKET packet;
        packet.id = MyPlayerCharacter->id;
        packet.type = CS_GRAVITYBOX_ADD;
        packet.size = sizeof CS_GRAVITYBOX_ADD_PACKET;
        packet.location.x = SpawnGravityBox->BoxLocation.X;
        packet.location.y = SpawnGravityBox->BoxLocation.Y;
        packet.location.z = SpawnGravityBox->BoxLocation.Z;
        packet.rotation.x = SpawnGravityBox->BoxRotation.Yaw;
        packet.rotation.y = SpawnGravityBox->BoxRotation.Pitch;
        packet.rotation.z = SpawnGravityBox->BoxRotation.Roll;

        int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

        UE_LOG(LogTemp, Warning, TEXT("GravityBox Spawned"));
        ++nGravityBox;
        return true;
    }
	else {
		return false;
	}
}

void UTIMERUNGameInstance::SpawnGravityBox()
{
	
}


void UTIMERUNGameInstance::UpdateNewGravityBox(FVector location, FRotator rotation, int box_id)
{
    UWorld* const world = GetWorld();
    AGravityBox* SpawnGravityBox = world->SpawnActor<AGravityBox>(location, rotation);
    SpawnGravityBox->BoxId = box_id;
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

