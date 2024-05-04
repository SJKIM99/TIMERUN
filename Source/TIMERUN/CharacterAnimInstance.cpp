// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TIMERUNCharacter.h"
#include "Kismet/GameplayStatics.h"

UCharacterAnimInstance::UCharacterAnimInstance()
{
	MoveSpeed = 0.f;
	ShouldMove = false;
	IsFalling = false;


	HaveGravityGun = false;
	IsGrabbingObject = false;
	HaveTimeMachine = false;
	IsTimeChanging = false;
	Direction = 0.f;

}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	
	if (::IsValid(Pawn))
	{
		ATIMERUNCharacter* MyPlayerCharacter = Cast<ATIMERUNCharacter>(Pawn);

		Velocity = MyPlayerCharacter->GetVelocity();

		MoveSpeed = MyPlayerCharacter->GetVelocity().Size();

		if (MoveSpeed > 3.f) ShouldMove = true;
		else ShouldMove = false;

		isLanded = MyPlayerCharacter->isLanded;
		IsFalling = !isLanded;

		HaveGravityGun = MyPlayerCharacter->HaveGravityGun;

		HaveTimeMachine = MyPlayerCharacter->HaveTimeMachine;

		IsGrabbingObject = MyPlayerCharacter->IsGrabbingObject;

		Direction = CalculateDirection(MyPlayerCharacter->GetVelocity(), MyPlayerCharacter->GetActorRotation());
		
	}
	else UE_LOG(LogTemp, Warning , TEXT("Pawn is not valid"));


}
