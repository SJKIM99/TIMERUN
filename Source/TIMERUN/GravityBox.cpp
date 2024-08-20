// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityBox.h"
#include "Kismet/GameplayStatics.h"
#include "TIMERUNGameInstance.h"
#include "TimerManager.h"

// Sets default values
AGravityBox::AGravityBox()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    RootComponent = StaticMeshComponent;

    // Load Static Mesh Asset
    static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Game/GravityBox/Resource/GravityBox'"));
    if (StaticMeshAsset.Succeeded())
    {
        StaticMeshComponent->SetStaticMesh(StaticMeshAsset.Object);
    }

    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(StaticMeshComponent); // Attach to Static Mesh Component
   
    LazerEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LazerEffect"));
    if (LazerEffect)
    {
        // Load the Niagara system from the specified path
        static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(TEXT("/Game/Effects/Laser.Laser"));

        if (NiagaraSystemAsset.Succeeded())
        {
            LazerEffect->SetAsset(NiagaraSystemAsset.Object);
            LazerEffect->Activate(true);  // Optionally activate the component
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load Niagara system asset."));
        }
    }

    // Load Skeletal Mesh Asset
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("SkeletalMesh'/Game/GravityBox/Resource/GravityBox_Rigged'"));
    if (SkeletalMeshAsset.Succeeded())
    {
        SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
    }

    //�ִϸ��̼� ��������� ����
    SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationClass(TEXT("/Game/GravityBox/Resource/Animation/BP_GravityBoxAnimInstance_C"));
    if (AnimationClass.Succeeded())
    {
        //�ִϸ��̼� �������Ʈ Ŭ������ �����ͼ� ����
        SkeletalMeshComponent->SetAnimInstanceClass(AnimationClass.Class);
    }



    isGrabbed = false;
    CanFixPos = false;
    SpawnTime = 1.f;


    //�⺻ ����ƽ �޽� ����
    StaticMeshComponent->SetHiddenInGame(true, true);
    StaticMeshComponent->SetCastShadow(false);

    SkeletalMeshComponent->bRenderCustomDepth = true;
    SkeletalMeshComponent->CustomDepthStencilValue = 0;

    //StaticMeshComponent->SetMassScale(NAME_None, 10.f);
    StaticMeshComponent->SetSimulatePhysics(true); // Enable physics simulation for the static mesh
    StaticMeshComponent->SetLinearDamping(1.f);

    timestate_location.Init(FVector::ZeroVector, TIMESIZE);
    timestate_rotation.Init(FRotator::ZeroRotator, TIMESIZE);

    StaticMeshComponent->BodyInstance.bOverrideMass = true;
    StaticMeshComponent->BodyInstance.SetMassOverride(10.f);
}

// Called when the game starts or when spawned
void AGravityBox::BeginPlay()
{
    Super::BeginPlay();

    //ConnectLaser();

    StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();

    instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());
    instance->GetSocketMgr()->GetIngameSocket();

    GetWorld()->GetTimerManager().SetTimer(SendGravityBoxInfoHandle, this, &AGravityBox::SendGravityBoxMovePacket, 0.1f, true);

    TimeLevel = 0.f;
    SkeletalMeshComponent = FindComponentByClass<USkeletalMeshComponent>();
    if (SkeletalMeshComponent)
    {
        // Assuming the material index is 0, change it according to your setup
        UMaterialInterface* Material = SkeletalMeshComponent->GetMaterial(0);
        if (Material)
        {
            // Check if the material is dynamic
            DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
            if (!DynamicMaterial)
            {
                // If not dynamic, create a dynamic instance
                DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
                SkeletalMeshComponent->SetMaterial(0, DynamicMaterial);
            }
        }
    }
}

// Called every frame
void AGravityBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    BoxLocation = GetActorLocation(); //�ڽ��� ��ġ��, FVector���·� ��
    BoxRotation = GetActorRotation(); //�ڽ��� ȸ����, FRotator���·� ��

    IsMoving = IsMovingCheck();
    CanFallCheck();
    DoGrabbingRotate(isGrabbed);

    FName ParameterName = FName("MaskSpread"); // Name of the parameter
    DynamicMaterial->SetScalarParameterValue(ParameterName, TimeLevel);

    ParameterName = FName("ShowDown Value"); // Name of the parameter
    DynamicMaterial->SetScalarParameterValue(ParameterName, SpawnTime);
    if (SpawnTime > 0) SpawnTime -= GetWorld()->DeltaTimeSeconds;

    CanFixPos = CanFixPosCheck();
    if (CanFixPos == true) {
        if (OneTimeSend == false) {
            SendGravityBoxMovePacket();
            ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

            for (int i = MyPlayerCharacter->my_time; i < TIMESIZE; ++i) {
                timestate_location[i].X = BoxLocation.X;
                timestate_location[i].Y = BoxLocation.Y;
                timestate_location[i].Z = BoxLocation.Z;

                timestate_rotation[i].Yaw = BoxRotation.Yaw;
                timestate_rotation[i].Pitch = BoxRotation.Pitch;
                timestate_rotation[i].Roll = BoxRotation.Roll;
            }

            if (ByWhoID == instance->my_id) {
                CS_GRAVITYBOX_TIME_STATE_PACKET packet;
                packet.size = sizeof CS_GRAVITYBOX_TIME_STATE_PACKET;
                packet.type = CS_GRAVITYBOX_TIME_STATE;
                packet.boxid = BoxId;
                packet.my_time = MyPlayerCharacter->my_time;
                packet.location.x = BoxLocation.X;
                packet.location.y = BoxLocation.Y;
                packet.location.z = BoxLocation.Z;
                packet.rotation.x = BoxRotation.Yaw;
                packet.rotation.y = BoxRotation.Pitch;
                packet.rotation.z = BoxRotation.Roll;

                send(*instance->ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
            }
            OneTimeSend = true;
        }
    }
    else {
        OneTimeSend = false;
    }
}

// Called to bind functionality to input
void AGravityBox::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AGravityBox::IsMovingCheck()
{
    if (GetVelocity().Size() > 1.f) return true;
    else return false;
}

void AGravityBox::CanFallCheck()
{
    // �浹 �˻� �������� ���� ����
    FVector StartLocation = GetActorLocation();
    FVector EndLocation = StartLocation + FVector(0.f, 0.f, -70.f); // TraceDistance�� float ������ �����ؾ� �մϴ�.

    // �浹 �˻縦 ���� �Ķ���� ����
    FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, GetOwner());

    // ����� ���� ����
    FHitResult HitResult;

    // �浹 �˻� ����
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,      // �浹 ����� ������ ����
        StartLocation,  // ������
        EndLocation,    // ����
        ECC_Visibility, // ä�� (ECC_Visibility ��� �ٸ� ä���� ����� �� �ֽ��ϴ�.)
        TraceParams     // �浹 �˻� �Ķ����
    );

    // �浹 ���� Ȯ��
    if (bHit)
    {
        CanFall = false;
    }
    else
    {
        CanFall = true;
    }


    // ���������� �浹 �˻� ����� �׸� �� �ֽ��ϴ�.
    //DrawDebugLine(
    //    GetWorld(),
    //    StartLocation,
    //    EndLocation,
    //    FColor::Red, // ���� ����
    //    true,// ���������� �׸� ������ ����
    //    0.f, // ���� �β�
    //    0, // DepthPriority
    //    0.f // ���ӽð� (0�� ���������� �׸�)
    //);

}

bool AGravityBox::CanFixPosCheck()
{

    if (!IsMoving && !isGrabbed && !CanFall)
    {
        if (StaticMeshComponent)
        {
            StaticMeshComponent->SetSimulatePhysics(false);
        }
        return true;
    }
    else
    {
        if (StaticMeshComponent)
        {
            StaticMeshComponent->SetSimulatePhysics(true);
        }
        return false;
    }
}

void AGravityBox::DoGrabbingRotate(bool when)
{
    
    if (StaticMeshComponent && when)
    {
        //StaticMeshComponent->SetEnableGravity(false);

        if (instance->my_id == ByWhoID)
        {
            FRotator RotationValue(0.f, 0.05f, 0.05f);
            StaticMeshComponent->AddWorldRotation(RotationValue);
        }
        else
        {
         /*   StaticMeshComponent->SetCollisionProfileName("NoCollision");
            StaticMeshComponent->SetSimulatePhysics(false);*/
            StaticMeshComponent->SetSimulatePhysics(false);
            StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    else if (StaticMeshComponent && !when) {
     /*   StaticMeshComponent->SetEnableGravity(true);
        StaticMeshComponent->SetCollisionProfileName("BlockAll");
        StaticMeshComponent->SetSimulatePhysics(true);*/
        StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        //StaticMeshComponent->SetSimulatePhysics(true);
    
    }
}

void AGravityBox::ConnectLaser()
{
    LazerEffect->SetVariableVec3(FName("Beam End"), GetActorLocation());
}

void AGravityBox::SendGravityBoxMovePacket()
{
    if (!instance->GameStart) return;
    ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    //if (MyPlayerCharacter->my_time != OhterPlayerCharacter->my_time) return;

    if (ByWhoID == instance->my_id) {
        if (!CanFixPos) {

            CS_GRAVITYBOX_UPDATE_PACKET packet;
            packet.type = CS_GRAVITYBOX_UPDATE;
            packet.size = sizeof CS_GRAVITYBOX_UPDATE_PACKET;
            packet.bywhoid = ByWhoID;
            packet.boxid = BoxId;
            packet.isgrabbed = isGrabbed;
            packet.location.x = BoxLocation.X;
            packet.location.y = BoxLocation.Y;
            packet.location.z = BoxLocation.Z;
            packet.rotation.x = BoxRotation.Yaw;
            packet.rotation.y = BoxRotation.Pitch;
            packet.rotation.z = BoxRotation.Roll;
            packet.velocity.x = GetVelocity().X;
            packet.velocity.y = GetVelocity().Y;
            packet.velocity.z = GetVelocity().Z;
            packet.time = box_time;
            packet.grabbed_time = MyPlayerCharacter->my_time;
            packet.ismoving = IsMoving;
            packet.canfixpos = CanFixPos;
            packet.canfall = CanFall;

            if (instance->ingame_socket == NULL) return;
            int ret = send(*instance->ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
        }
    }
}

void AGravityBox::SendGravityBoxGrabbedPacket()
{
    if (!instance->GameStart) return;

    CS_GRAVITYBOX_GRABBED_PACKET packet;
    packet.size = sizeof CS_GRAVITYBOX_GRABBED_PACKET;
    packet.type = CS_GRAVITYBOX_GRABBED;
    packet.boxid = BoxId;
    packet.isGrabbed = true;

    if (instance->ingame_socket == NULL) return;
    int ret = send(*instance->ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}

void AGravityBox::SendGravityBoxDroppedPacket()
{
    if (!instance->GameStart) return; 

    CS_GRAVITYBOX_DROPPED_PACKET packet;
    packet.size = sizeof CS_GRAVITYBOX_DROPPED_PACKET;
    packet.type = CS_GRAVITYBOX_DROPPED;
    packet.boxid = BoxId;
    packet.isGrabbed = false;

    if (instance->ingame_socket == NULL) return;
    int ret = send(*instance->ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
}
