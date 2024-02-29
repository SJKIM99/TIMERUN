// Fill out your copyright notice in the Description page of Project Settings.

#include "TIMERUNPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include <string>

#define MOUSE_SENSITIVE 100.f

ATIMERUNPlayerController::ATIMERUNPlayerController()
{
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (players[i] == nullptr) {
            players[i] = new Session();
        }
    }
}

ATIMERUNPlayerController::~ATIMERUNPlayerController()
{
}

void ATIMERUNPlayerController::BeginPlay()
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

void ATIMERUNPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsActiveIngameSocket)
		RecvPacketFromLoginServer();
	else
		RecvPacketFromIngameServer();
}

void ATIMERUNPlayerController::RecvPacketFromLoginServer()
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

void ATIMERUNPlayerController::RecvPacketFromIngameServer()
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


void ATIMERUNPlayerController::ProcessPakcet(char* packet)
{
	switch (packet[1]) {
	case SC_LOGIN_SUCCESS: {
		SC_LOGIN_SUCCESS_PACKET* p = reinterpret_cast<SC_LOGIN_SUCCESS_PACKET*>(packet);

        auto myplayer = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        myplayer->id = p->id;
        memcpy(myplayer->nickname, p->nickname, sizeof p->nickname);
        
		instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

		instance->GetSocketMgr()->ConnectIngameServer();

		ingame_socket = instance->GetSocketMgr()->GetIngameSocket();

        CS_INGAME_LOGIN_PACKET packet;
        packet.size = sizeof CS_INGAME_LOGIN_PACKET;
        packet.type = CS_INGAME_LOGIN;
        packet.id = p->id;

        int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);

		IsActiveIngameSocket = true;
	}
						 break;

	case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
        
        UpdateNewPlayer(p->id);
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
        players[p->id]->location.x = p->location.x;
        players[p->id]->location.y = p->location.y;
        players[p->id]->location.z = p->location.z;

        players[p->id]->Yaw = p->yaw;

        FVector CharacterLocation;
        CharacterLocation.X = p->location.x;
        CharacterLocation.Y = p->location.y;
        CharacterLocation.Z = p->location.z;

        FRotator CharacterRotation;
        CharacterRotation.Yaw = p->yaw;
        CharacterRotation.Pitch = 0;
        CharacterRotation.Roll = 0;

        TArray<AActor*> spawnedCharacters;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATIMERUNCharacter::StaticClass(), spawnedCharacters);
        spawnedCharacters[p->id]->SetActorLocation(CharacterLocation);
        spawnedCharacters[p->id]->SetActorRotation(CharacterRotation);

        UE_LOG(LogTemp, Warning, TEXT("Player %d moved to (%f, %f, %f)"), p->id, p->location.x, p->location.y, p->location.z);
    }
					   break;
	}
}

void ATIMERUNPlayerController::SendMovePacket(direction direction, APawn* pawn)
{
    CS_MOVE_PACKET packet;
    packet.size = sizeof CS_MOVE_PACKET;
    packet.type = CS_MOVE;
    packet.direction = direction;
    packet.yaw = pawn->GetActorRotation().Yaw;
    packet.location.x= pawn->GetActorLocation().X;
    packet.location.y= pawn->GetActorLocation().Y;
    packet.location.z= pawn->GetActorLocation().Z;

    int ret = send(*ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}

void ATIMERUNPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 이동
    InputComponent->BindAxis(TEXT("MoveForward"), this, &ATIMERUNPlayerController::MoveForward);
    InputComponent->BindAxis(TEXT("MoveRight"), this, &ATIMERUNPlayerController::MoveRight);
    InputComponent->BindAxis(TEXT("MoveLeft"), this, &ATIMERUNPlayerController::MoveLeft);
    InputComponent->BindAxis(TEXT("MoveBack"), this, &ATIMERUNPlayerController::MoveBack);

    // 마우스 움직임 바인딩
    InputComponent->BindAxis(TEXT("LookUp"), this, &ATIMERUNPlayerController::LookUp); // 마우스의 세로 이동
    InputComponent->BindAxis(TEXT("Turn"), this, &ATIMERUNPlayerController::Turn); // 마우스의 가로 이동

    // 점프 바인딩
    InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATIMERUNPlayerController::Jump);
}

void ATIMERUNPlayerController::Turn(float NewAxisValue)
{
    float delta_time = GetWorld()->GetDeltaSeconds();
    AddYawInput(delta_time * NewAxisValue * MOUSE_SENSITIVE);
}

void ATIMERUNPlayerController::LookUp(float NewAxisValue)
{
    float delta_time = GetWorld()->GetDeltaSeconds();
    AddPitchInput(delta_time * NewAxisValue * MOUSE_SENSITIVE);
}

void ATIMERUNPlayerController::MoveForward(float Value)
{
    // Check if the value is not zero (W key is pressed)
    if (Value != 0.f)
    {
        // Get the controlled pawn (your character)
        APawn* ControlledPawn = GetPawn();

        // Check if the controlled pawn exists
        if (ControlledPawn)
        {
           /* players[my_id].location.x = ControlledPawn->GetActorLocation().X;
            players[my_id].location.y = ControlledPawn->GetActorLocation().Y;
            players[my_id].location.z = ControlledPawn->GetActorLocation().Z;*/
            // Extract the yaw value
            float Yaw = GetControlRotation().Yaw;

            // Convert the yaw value to a rotation matrix to get the forward vector
            FRotator Rotation(0.f, Yaw, 0.f);
            FVector ForwardVector = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

            // Move the pawn backward
            ControlledPawn->AddMovementInput(ForwardVector, Value); // Negate the ForwardVector to move backward
            SendMovePacket(direction::forward,ControlledPawn);
        }
    }
}

void ATIMERUNPlayerController::MoveRight(float Value)
{
    // Check if the value is not zero (D key is pressed)
    if (Value != 0.f)
    {
        // Get the controlled pawn (your character)
        APawn* ControlledPawn = GetPawn();

        // Check if the controlled pawn exists
        if (ControlledPawn)
        {
            // Move the pawn to the right
            FVector RightVector = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::Y);
            ControlledPawn->AddMovementInput(RightVector, Value);
            SendMovePacket(direction::right, ControlledPawn);
        }
    }
}

void ATIMERUNPlayerController::MoveLeft(float Value)
{
    // Check if the value is not zero (A key is pressed)
    if (Value != 0.f)
    {
        // Get the controlled pawn (your character)
        APawn* ControlledPawn = GetPawn();

        // Check if the controlled pawn exists
        if (ControlledPawn)
        {
            // Move the pawn to the left
            FVector RightVector = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::Y);
            ControlledPawn->AddMovementInput(-RightVector, Value); // Negate the RightVector to move left
            SendMovePacket(direction::left, ControlledPawn);
        }
    }
}

void ATIMERUNPlayerController::MoveBack(float Value)
{
    // Check if the value is not zero (S key is pressed)
    if (Value != 0.f)
    {
        // Get the controlled pawn (your character)
        APawn* ControlledPawn = GetPawn();

        // Check if the controlled pawn exists
        if (ControlledPawn)
        {
            // Extract the yaw value
            float Yaw = GetControlRotation().Yaw;

            // Convert the yaw value to a rotation matrix to get the forward vector
            FRotator Rotation(0.f, Yaw, 0.f);
            FVector ForwardVector = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

            // Move the pawn backward
            ControlledPawn->AddMovementInput(-ForwardVector, Value); // Negate the ForwardVector to move backward
            SendMovePacket(direction::back, ControlledPawn);
        }
    }
}

void ATIMERUNPlayerController::Jump()
{
    // Get the controlled pawn (your character)
    APawn* ControlledPawn = GetPawn();

    // Check if the controlled pawn exists and is a character
    ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
    if (ControlledCharacter)
    {
        // Check if the character can jump
        if (ControlledCharacter->CanJump())
        {
            // Perform the jump
            ControlledCharacter->Jump();
        }
    }
}

void ATIMERUNPlayerController::UpdateNewPlayer(int c_id)
{
    UWorld* const world = GetWorld();
    /*if (c_id == my_id) {
        IsEnterNewPlayer = false;
        NewPlayer = nullptr;
        return;
    }*/
 /*   FVector spawnlocation;
    spawnlocation.X = NewPlayer->location.x;
    spawnlocation.Y = NewPlayer->location.y;
    spawnlocation.Z = NewPlayer->location.z;

    FRotator spawnrotation;
    spawnrotation.Yaw = NewPlayer->Yaw;*/

    /*FActorSpawnParameters spawnparams;
    spawnparams.Owner = this;
    spawnparams.Instigator = Instigator;*/

    ATIMERUNCharacter* SpawnCharacter = world->SpawnActor<ATIMERUNCharacter>();
    SpawnCharacter->SpawnDefaultController();
    SpawnCharacter->id = c_id;
    
    if (players[c_id] != nullptr) {
        Session player;
        player.location.x = 0;
        player.location.y = 0;
        player.location.z = 0;

        player.Yaw = 0;

        players[c_id] = &player;
    }
    IsEnterNewPlayer = false;
}
