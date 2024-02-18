// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameModeBase.h"
#include "TIMERUNCharacter.h"
#include "TIMERUNPlayerController.h"

ATIMERUNGameModeBase::ATIMERUNGameModeBase()
{
	DefaultPawnClass = ATIMERUNCharacter::StaticClass();
	DefaultPawnClass = ATIMERUNCharacter::StaticClass();

	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_TIMERUNCharacter"));
	//DefaultPawnClass = PlayerPawnBPClass.Class;
	PlayerControllerClass = ATIMERUNPlayerController::StaticClass();
}
 