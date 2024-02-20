// Fill out your copyright notice in the Description page of Project Settings.

#include "TIMERUNPlayerController.h"

void ATIMERUNPlayerController::BeginPlay()
{
	Super::BeginPlay();

    instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());

    client_socket = instance->GetSocketMgr()->GetSocket();

    CS_LOGIN_PACKET packet;
    packet.size = sizeof CS_LOGIN_PACKET;
    packet.type = CS_LOGIN;
    
    strcpy_s(packet.id, "asd");

    int ret = send(*client_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}

void ATIMERUNPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    RecvPacket();
}

void ATIMERUNPlayerController::RecvPacket()
{
    char buf[BUF_SIZE];
    int ret = recv(*client_socket, reinterpret_cast<char*>(&buf), BUF_SIZE, 0);
    if (ret <= 0) {
        UE_LOG(LogTemp, Warning, TEXT("Recv Fail"));
        return;
    }
    else if (ret > BUF_SIZE - prev_remain_data) {
        UE_LOG(LogTemp, Warning, TEXT("overflow data, recv data size: %d"), ret);
        prev_remain_data = 0;
        return;
    }

    if (prev_remain_data > 0) {
        UE_LOG(LogTemp, Warning, TEXT("exist prev remin data"));
        memcpy(prev_packet_buf + prev_remain_data, buf, ret);
    }
    else {
        memcpy(prev_packet_buf, buf, ret);
    }

    int remain_data = ret + prev_remain_data;
    char* p = prev_packet_buf;
    while (remain_data > 0) {
        int packet_size = p[0];
        if (packet_size == 0) {
            prev_remain_data = 0;
            return;
        }
        if (packet_size <= remain_data) {
            ProcessPakcet(p);
            p += packet_size;
            remain_data -= packet_size;
        }
        else break;
    }
    prev_remain_data = remain_data;
    if (remain_data > 0) {
        memcpy(prev_packet_buf, p, remain_data);
    }
}

void ATIMERUNPlayerController::ProcessPakcet(char* packet)
{
    switch (packet[1]) {
    case SC_LOGIN_SUCCESS: {
        SC_LOGIN_SUCCESS_PACKET* packet = new SC_LOGIN_SUCCESS_PACKET;
        UE_LOG(LogTemp, Warning, TEXT("Recv SC_LOGIN_SUCESS_PACKET"));
        
    }
                         break;
    case SC_LOGIN_FAIL: {

    }
                      break;
    case SC_SIGNUP: {
        SC_SIGNUP_PACKET* packet = new SC_SIGNUP_PACKET;
        UE_LOG(LogTemp, Warning, TEXT("Recv SC_SIGNUP_PACKET"));
    }
                  break;
    }
}

void ATIMERUNPlayerController::SendMovePacket(direction direction)
{
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
    AddYawInput(delta_time * NewAxisValue * 20.0f);
}

void ATIMERUNPlayerController::LookUp(float NewAxisValue)
{
    float delta_time = GetWorld()->GetDeltaSeconds();
    AddPitchInput(delta_time * NewAxisValue * 20.0f);
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
            // Get the forward vector of the controller rotation
            FVector ForwardVector = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);

            // Move the pawn forward
            ControlledPawn->AddMovementInput(ForwardVector, Value);
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
            // Move the pawn backward
            FVector ForwardVector = FRotationMatrix(GetControlRotation()).GetScaledAxis(EAxis::X);
            ControlledPawn->AddMovementInput(-ForwardVector, Value); // Negate the ForwardVector to move backward
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
