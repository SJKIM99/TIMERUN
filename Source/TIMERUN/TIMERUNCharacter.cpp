// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "TIMERUNGameInstance.h"
#include "Materials/MaterialInstanceDynamic.h"




// Sets default values
ATIMERUNCharacter::ATIMERUNCharacter() : my_time(0)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //플레이어 케릭터
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Player/Resource/Male_Rig"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object);
        GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
    }

    //애니메이션 블루프린터 연결
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationClass(TEXT("/Game/Player/Resource/Anim/AI_Character"));
    if (AnimationClass.Succeeded())
    {
        //애니메이션 블루프린트 클래스를 가져와서 설정
        GetMesh()->SetAnimInstanceClass(AnimationClass.Class);
    }
    GetMesh()->bRenderCustomDepth = true;
    GetMesh()->CustomDepthStencilValue = 1;

    // 중력총
    GravityGunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GravityGun"));
    if (GravityGunMesh)
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Game/GravityGun/GravityGun"));
        if (StaticMeshAsset.Succeeded())
        {
            GravityGunMesh->SetStaticMesh(StaticMeshAsset.Object);
            GravityGunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        FName GravityGunSocket(TEXT("GravityGunSocket"));
        GravityGunMesh->SetupAttachment(GetMesh(), GravityGunSocket);
    }

    //중력총 총구 REF
    Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle"));
    Muzzle->ArrowSize = 0.5f;
    Muzzle->ArrowColor = FColor::Blue;

    //변수 초기화
    WalkSpeed = 250.f;
    RunSpeed = 600.f;
    GunWalkSpeed = 150.f;

    GravityGunRange = 300.f;
    HaveGravityGun = false;
    IsGrabbingObject = false;

    HaveCameraShot = false;
    MyScore = false;

    HaveTimeMachine = false;
    DoingTimeTravel = false;

    //캐릭터 기본 세팅
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->BrakingFrictionFactor = 0.01;
    GetCharacterMovement()->JumpZVelocity = 500.f;
}

// Called when the game starts or when spawned
void ATIMERUNCharacter::BeginPlay()
{
    Super::BeginPlay();
    GravityGunMesh = FindComponentByClass<UStaticMeshComponent>();


    //중력총 설정
    Muzzle = FindComponentByClass<UArrowComponent>();
    Muzzle->AttachToComponent(GravityGunMesh, FAttachmentTransformRules::KeepRelativeTransform);
    FVector MuzzleLocation(0.f, 20.5f, 4.5f);
    FRotator MuzzleRotation(0.0f, 90.0f, 0.f);
    Muzzle->SetRelativeLocation(MuzzleLocation);
    Muzzle->SetRelativeRotation(MuzzleRotation);


    instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());
    instance->GetSocketMgr()->GetIngameSocket();

    SkillCoolTime = SPAWN_GRAVITYBOX_COOLTIME;

    InitializeDynamicMaterials();
}

// Called every frame
void ATIMERUNCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //UpdateGravityGunVisibility();
    DoJump();

    if (HaveGravityGun)
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->MaxWalkSpeed = GunWalkSpeed;
    }
    else
    {
        GetCharacterMovement()->bOrientRotationToMovement = true;
        //GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    }
}

// Called to bind functionality to input
void ATIMERUNCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);


}


void ATIMERUNCharacter::UpdateGravityGunVisibility()
{
    if (GravityGunMesh) {
        GravityGunMesh->SetHiddenInGame(!HaveGravityGun);
    }
}

void ATIMERUNCharacter::DoJump()
{
    /*if (IsJump)
    {
       Jump();
    }

    IsJump = false;*/
}

void ATIMERUNCharacter::InitializeDynamicMaterials()
{
    const int32 MaterialCount = GetMesh()->GetNumMaterials();
    DMIs_Character.SetNum(MaterialCount);

    // Create dynamic material instances for each material slot
    for (int32 i = 0; i < MaterialCount; ++i)
    {
        UMaterialInterface* Material = GetMesh()->GetMaterial(i);
        if (Material)
        {
            DMIs_Character[i] = UMaterialInstanceDynamic::Create(Material, this);
            GetMesh()->SetMaterial(i, DMIs_Character[i]);
        }
    }
}

