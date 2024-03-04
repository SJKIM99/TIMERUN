// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"

UCharacterAnimInstance::UCharacterAnimInstance()
{
	MoveSpeed = 0.f;
	IsFalling = false;
	HaveGravityGun = false;
	IsGrabbingObject = false;
	Direction = 0;
}