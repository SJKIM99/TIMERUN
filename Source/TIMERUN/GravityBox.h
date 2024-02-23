// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravityBox.generated.h"

UCLASS()
class TIMERUN_API AGravityBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//스테틱 메쉬 컴포넌트 선언
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	////스켈레탈 메쉬 컴포넌트 선언
	//UPROPERTY(EditAnywhere)
	//USkeletalMeshComponent* SkeletalMeshComponent;


	//이 엑터가 움직이고 있는지 확인해주는 함수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMoving; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
	bool isGrapping; // 중력총을 들었는지 안들었는지

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터, 블루프린트 안에서 수정 가능
	FTransform GravityBoxTransform; //GravityB
};
