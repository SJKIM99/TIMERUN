// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityBoxAnimInstance.h"
#include "GravityBox.h"

UGravityBoxAnimInstance::UGravityBoxAnimInstance()
{
	CanFixPos = true;
}

void UGravityBoxAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		AGravityBox* GravityBox = Cast<AGravityBox>(Pawn);

		CanFixPos = GravityBox->CanFixPos;
	}
}
