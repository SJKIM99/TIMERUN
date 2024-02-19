// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "TIMERUNPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUN_API ATIMERUNPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupInputComponent() override;
private:
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveLeft(float Value);
	void MoveBack(float Value);

	void Jump();
};
