// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimInstance* AnimInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GunWalkSpeed;



	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GravityGunRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveGravityGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGrabbingObject;

public:
	int id;
	char nickname[NAME_SIZE];
};
