// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameModeBase.h"
#include "TIMERUNCharacter.h"
#include "TIMERUNPlayerController.h"

ATIMERUNGameModeBase::ATIMERUNGameModeBase()
{
	DefaultPawnClass = ATIMERUNCharacter::StaticClass();
	PlayerControllerClass = ATIMERUNPlayerController::StaticClass();
}
 