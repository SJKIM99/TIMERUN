// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TIMERUNCharacter.generated.h"

class UTIMERUNGameInstance;

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

	void UpdateGravityGunVisibility();

	void DoJump();


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* GravityGunMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow_REF")
	UArrowComponent* Muzzle;


public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsJump;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GunWalkSpeed;



	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GravityGunRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveGravityGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGrabbingObject;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveCameraShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MyScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanTakePicture;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveTimeMachine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DoingTimeTravel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isLanded = false;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString nickname;
	//보간때 쓰이는 변수들
	FVector current_location;
	FRotator current_rotation;
	FVector current_velocity;

	FVector prev_location;
	FRotator prev_rotation;
	FVector prev_velocity;
public:
	//시간이동할때 쓰이는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int my_time;
public:
	UTIMERUNGameInstance* instance;
};