// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ATIMERUNCharacter::ATIMERUNCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Player/Resource/Male_Rigged"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	//애니메이션 블루프린터 연결
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationClass(TEXT("/Game/Player/Resource/Animation/BP_CharacterAnim"));
	if (AnimationClass.Succeeded())
	{
		//애니메이션 블루프린트 클래스를 가져와서 설정
		GetMesh()->SetAnimInstanceClass(AnimationClass.Class);
	}
	
	

	//변수 초기화
	WalkSpeed = 250.f;
	RunSpeed = 600.f;
	GunWalkSpeed = 150.f;

	GravityGunRange = 300.f;
	HaveGravityGun = false;
	IsGrabbingObject = false;

	//캐릭터 기본 세팅
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->BrakingFrictionFactor = 0.01;
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

