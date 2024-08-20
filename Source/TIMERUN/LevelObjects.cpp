// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelObjects.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ALevelObjects::ALevelObjects()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(StaticMeshComponent); // Attach to Static Mesh Component

	TimeLevel = 0;

}

// Called when the game starts or when spawned
void ALevelObjects::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = Cast<ATIMERUNCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	
	TimeLevel = (float)(MyCharacter->my_time) / 10.0;
}

// Called every frame
void ALevelObjects::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}


