// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNCharacter.h"

// Sets default values
ATIMERUNCharacter::ATIMERUNCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Spring Arm 생성
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
    SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f)); // 카메라 각도 조절

    // 카메라 생성 및 설정
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = true; // 카메라가 캐릭터의 회전을 따르지 않도록 설정

    GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Player/Resource/Male_Rigged"));
    if (MeshAsset.Succeeded()) {
        UE_LOG(LogTemp, Warning, TEXT("SkeletMesh Spawn Success"));
        TIMERUNMesh = MeshAsset.Object;
        GetMesh()->SetSkeletalMesh(TIMERUNMesh);
    }
}

// Called when the game starts or when spawned
void ATIMERUNCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATIMERUNCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATIMERUNCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

