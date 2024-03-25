// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityBox.h"
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

	// Load Skeletal Mesh Asset
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("SkeletalMesh'/Game/GravityBox/Resource/GravityBox_Rigged'"));
	if (SkeletalMeshAsset.Succeeded())
	{
	    SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

    //애니메이션 블루프린터 연결
    SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationClass(TEXT("/Game/GravityBox/Resource/Animation/BP_GravityBoxAnimInstance"));
    if (AnimationClass.Succeeded())
    {
    	//애니메이션 블루프린트 클래스를 가져와서 설정
        SkeletalMeshComponent->SetAnimInstanceClass(AnimationClass.Class);
    }


    isGrabbed = false;
    CanFixPos = false;

	//기본 스테틱 메쉬 설정
	StaticMeshComponent->SetHiddenInGame(true, true);
	StaticMeshComponent->SetCastHiddenShadow(true);

	StaticMeshComponent->SetMassScale(NAME_None, 10.f);
	StaticMeshComponent->SetSimulatePhysics(true); // Enable physics simulation for the static mesh
	StaticMeshComponent->SetLinearDamping(1.f);
}

// Called when the game starts or when spawned
void AGravityBox::BeginPlay()
{
	Super::BeginPlay();


    StaticMeshComponent = FindComponentByClass<UStaticMeshComponent>();

    instance = Cast<UTIMERUNGameInstance>(GetWorld()->GetGameInstance());
    instance->GetSocketMgr()->GetIngameSocket();

    GetWorld()->GetTimerManager().SetTimer(SendGravityBoxInfoHandle, this, &AGravityBox::SendGravityBoxMovePacket, 0.016f, true);
}

// Called every frame
void AGravityBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    BoxLocation = GetActorLocation(); //박스의 위치값, FVector형태로 들어감
    BoxRotation = GetActorRotation(); //박스의 회전값, FRotator형태로 들어감

    IsMoving = IsMovingCheck();
    CanFallCheck();
    DoGrabbingRotate(isGrabbed);

    if (isGrabbed) StaticMeshComponent->SetEnableGravity(false);
    else StaticMeshComponent->SetEnableGravity(true);

    CanFixPos = CanFixPosCheck();

    //OtherGrappedCheck();
}

// Called to bind functionality to input
void AGravityBox::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AGravityBox::IsMovingCheck()
{
	if (GetVelocity().Size() > 0.f) return true;
	else return false;
}

void AGravityBox::CanFallCheck()
{
    // 충돌 검사 시작점과 끝점 설정
    FVector StartLocation = GetActorLocation();
    FVector EndLocation = StartLocation + FVector(0.f, 0.f, -70.f); // TraceDistance는 float 변수로 설정해야 합니다.

    // 충돌 검사를 위한 파라미터 설정
    FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, GetOwner());

    // 결과를 담을 변수
    FHitResult HitResult;

    // 충돌 검사 수행
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,      // 충돌 결과를 저장할 변수
        StartLocation,  // 시작점
        EndLocation,    // 끝점
        ECC_Visibility, // 채널 (ECC_Visibility 대신 다른 채널을 사용할 수 있습니다.)
        TraceParams     // 충돌 검사 파라미터
    );

    // 충돌 여부 확인
    if (bHit)
    {
        CanFall = false;
    }
    else
    {
        CanFall = true;
    }

    // 디버깅용으로 충돌 검사 결과를 그릴 수 있습니다.
    DrawDebugLine(
        GetWorld(),
        StartLocation,
        EndLocation,
        FColor::Red, // 선의 색상
        false,// 지속적으로 그릴 것인지 여부
        0.f, // 라인 두께
        0, // DepthPriority
        0.f // 지속시간 (0은 지속적으로 그림)
    );
	
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
        FRotator RotationValue(0.f, 0.05f, 0.05f);
        StaticMeshComponent->AddWorldRotation(RotationValue);
    }
}



void AGravityBox::SendGravityBoxMovePacket()
{
    if (ByWhoID != NULL) {
        if (ByWhoID == instance->my_id) {
            if (!CanFixPos) {
                CS_GRAVITYBOX_UPDATE_PACKET packet;
                packet.type = CS_GRAVITYBOX_UPDATE;
                packet.size = sizeof CS_GRAVITYBOX_UPDATE_PACKET;
                packet.boxid = BoxId;
                packet.location.x = BoxLocation.X;
                packet.location.y = BoxLocation.Y;
                packet.location.z = BoxLocation.Z;
                packet.rotation.x = BoxRotation.Yaw;
                packet.rotation.y = BoxRotation.Pitch;
                packet.rotation.z = BoxRotation.Roll;
                packet.velocity.x = GetVelocity().X;
                packet.velocity.y = GetVelocity().Y;
                packet.velocity.z = GetVelocity().Z;

                UE_LOG(LogTemp, Warning, TEXT("my id : %d"), instance->my_id);
                UE_LOG(LogTemp, Warning, TEXT("Grabbrd id : %d"), ByWhoID);
                int ret = send(*instance->ingame_socket, reinterpret_cast<char*>(&packet), sizeof packet, 0);
            }
        }
        else {
            StaticMeshComponent->SetSimulatePhysics(false);
            UE_LOG(LogTemp, Warning, TEXT("sexsex"));
        }
    }
}


