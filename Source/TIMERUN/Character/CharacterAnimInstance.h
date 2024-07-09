// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCharacterAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn, Meta = (AllowPrivateAccess = true))
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess=true))
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveGravityGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGrabbingObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveTimeMachine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsTimeChanging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isLanded;
};
