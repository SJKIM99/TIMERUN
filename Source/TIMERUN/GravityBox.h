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

	//스테틱 메쉬 컴포넌트 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	//스켈레탈 메쉬 컴포넌트 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLevel;

	//이 엑터가 움직이고 있는지 확인해주는 함수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
		bool isGrabbed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
		bool CanFall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
		bool CanFixPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
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
	FTimerHandle SendGravityBoxInfoHandle;   // 동기화 타이머 핸들러
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
	//시간이동할 때 쓰이는 변수
	int box_time;

	TArray<FVector> timestate_location;
	TArray<FRotator> timestate_rotation;
public:
	bool OneTimeSend = false;
};