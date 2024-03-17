// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GravityBoxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API UGravityBoxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UGravityBoxAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess))
	bool CanFixPos;
};
