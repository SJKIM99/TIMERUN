// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BeginWidget.generated.h"

/**
 * 
 */
class UTIMERUNGameInstance;
UCLASS()
class TIMERUN_API UBeginWidget : public UUserWidget
{
	GENERATED_BODY()

	UTIMERUNGameInstance* instance;
protected:
	virtual void NativeOnInitialized();
public:
	UPROPERTY(BlueprintReadWrite)
	FString ID;
	UPROPERTY(BlueprintReadWrite)
	FString PASSWD;
	UPROPERTY(BlueprintReadWrite)
	FString NICKNAME;

public:
	UFUNCTION(BlueprintCallable)
	bool SendLoginPacket();
	UFUNCTION(BlueprintCallable)
	bool SendSignUpPacket();
};
