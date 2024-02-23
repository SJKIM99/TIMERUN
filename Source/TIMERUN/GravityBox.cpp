// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityBox.h"

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

	//SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	//SkeletalMeshComponent->SetupAttachment(StaticMeshComponent); // Attach to Static Mesh Component

	//// Load Skeletal Mesh Asset
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("SkeletalMesh'/Game/GravityBox/Resource/GravityBox_Rigged'"));
	//if (SkeletalMeshAsset.Succeeded())
	//{
	//    SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
	//}



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
	
}

// Called every frame
void AGravityBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	GravityBoxTransform = GetActorTransform();

	if (GetVelocity() == FVector::ZeroVector) IsMoving = false;
	else IsMoving = true;

}

// Called to bind functionality to input
void AGravityBox::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

