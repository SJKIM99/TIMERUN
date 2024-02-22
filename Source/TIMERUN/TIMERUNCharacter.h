// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "TIMERUNCharacter.generated.h"

UCLASS()
class TIMERUN_API ATIMERUNCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATIMERUNCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* TIMERUNMesh; // 외부에서 가져온 메시를 저장하기 위한 변수
};
