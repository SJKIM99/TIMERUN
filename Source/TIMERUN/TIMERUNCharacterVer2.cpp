// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNCharacterVer2.h"

// Sets default values
ATIMERUNCharacterVer2::ATIMERUNCharacterVer2()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetActorHiddenInGame(false);
	UE_LOG(LogTemp, Warning, TEXT("SkeletMesh Spawn Success"));
}

// Called when the game starts or when spawned
void ATIMERUNCharacterVer2::BeginPlay()
{
	Super::BeginPlay();
	
	HaveGravityGun = false;
}

// Called every frame
void ATIMERUNCharacterVer2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATIMERUNCharacterVer2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

