// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TIMERUNCharacterVer2.generated.h"

UCLASS()
class TIMERUN_API ATIMERUNCharacterVer2 : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATIMERUNCharacterVer2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
	bool HaveGravityGun; // 중력총을 들었는지 안들었는지
};
