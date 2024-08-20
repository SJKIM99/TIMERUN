// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "C:/Users/user/Desktop/TIMERUN/LOGIN_SERVER/protocol.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "TIMERUNCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GravityBox.generated.h"



class UTIMERUNGameInstance;

UCLASS()
class TIMERUN_API AGravityBox : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGravityBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	bool IsMovingCheck();
	void CanFallCheck();
	bool CanFixPosCheck();
	void DoGrabbingRotate(bool when);

	void ConnectLaser();

	//����ƽ �޽� ������Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	//���̷�Ż �޽� ������Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLevel;

	//�� ���Ͱ� �����̰� �ִ��� Ȯ�����ִ� �Լ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ������, �������Ʈ �ȿ��� ���� ����
		bool isGrabbed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ������, �������Ʈ �ȿ��� ���� ����
		bool CanFall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ������, �������Ʈ �ȿ��� ���� ����
		bool CanFixPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // ������, �������Ʈ �ȿ��� ���� ����
		int ByWhoID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATIMERUNCharacter* ByWho;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraComponent* LazerEffect;


	float SpawnTime;

	FVector BoxLocation;
	FRotator BoxRotation;
	int BoxId;

	UTIMERUNGameInstance* instance;
	FTimerHandle SendGravityBoxInfoHandle;   // ����ȭ Ÿ�̸� �ڵ鷯
	void SendGravityBoxMovePacket();
	UFUNCTION(BlueprintCallable)
	void SendGravityBoxGrabbedPacket();
	UFUNCTION(BlueprintCallable)
	void SendGravityBoxDroppedPacket();
public:
	FTimerHandle MoveGravityBoxTimerHandle;

	FVector current_location;
	FRotator current_rotation;
	FVector current_velocity;
	FTimerDelegate TimerCallback;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//�ð��̵��� �� ���̴� ����
	int box_time;

	TArray<FVector> timestate_location;
	TArray<FRotator> timestate_rotation;
public:
	bool OneTimeSend = false;
};