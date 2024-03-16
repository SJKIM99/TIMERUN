// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "TIMERUNController.h"
#include "GravityBoxController.h"

void AGravityBoxController::BeginPlay()
{
    gravitybox->BoxLocation = gravitybox->GetActorLocation(); //박스의 위치값, FVector형태로 들어감
    gravitybox->BoxRotation = gravitybox->GetActorRotation(); //박스의 회전값, FRotator형태로 들어감

    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    CS_GRAVITYBOX_ADD_PACKET packet;
    packet.id = MyPlayerCharacter->id;
    packet.type = CS_GRAVITYBOX_ADD;
    packet.size = sizeof CS_GRAVITYBOX_ADD_PACKET;
    packet.location.x = gravitybox->BoxLocation.X;
    packet.location.y = gravitybox->BoxLocation.Y;
    packet.location.z = gravitybox->BoxLocation.Z;
    packet.rotation.x = gravitybox->BoxRotation.Yaw;
    packet.rotation.y = gravitybox->BoxRotation.Pitch;
    packet.rotation.z = gravitybox->BoxRotation.Roll;

    ATIMERUNController* MyController = Cast<ATIMERUNController>(GetWorld()->GetFirstPlayerController());

    int ret = send(MyController->GetIngmaeSocket(), reinterpret_cast<char*>(&packet), sizeof(packet), 0);

    UE_LOG(LogTemp, Warning, TEXT("GravityBox Spawned"));
}

void AGravityBoxController::Tick(float DeltaTime)
{
    if (!gravitybox->CanFixPos || !gravitybox->IsMoving) {
        gravitybox->BoxLocation = gravitybox->GetActorLocation(); //박스의 위치값, FVector형태로 들어감
        gravitybox->BoxRotation = gravitybox->GetActorRotation(); //박스의 회전값, FRotator형태로 들어감

        gravitybox->IsMoving = gravitybox->IsMovingCheck();
        gravitybox->CanFallCheck();
        gravitybox->CanFixPos = gravitybox->CanFixPosCheck();

        gravitybox->DoGrabbingRotate(gravitybox->isGrabbed);

        CS_GRAVITYBOX_UPDATE_PACKET packet;

        ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

        packet.id = MyPlayerCharacter->id;
        packet.boxid = gravitybox->BoxId;
        packet.type = CS_GRAVITYBOX_UPDATE;
        packet.size = sizeof CS_GRAVITYBOX_UPDATE_PACKET;
        packet.location.x = gravitybox->BoxLocation.X;
        packet.location.y = gravitybox->BoxLocation.Y;
        packet.location.z = gravitybox->BoxLocation.Z;
        packet.rotation.x = gravitybox->BoxRotation.Yaw;
        packet.rotation.x = gravitybox->BoxRotation.Pitch;
        packet.rotation.x = gravitybox->BoxRotation.Roll;

        ATIMERUNController* MyController = Cast<ATIMERUNController>(GetWorld()->GetFirstPlayerController());

        int ret = send(MyController->GetIngmaeSocket(), reinterpret_cast<char*>(&packet), sizeof(packet), 0);
    }
}
