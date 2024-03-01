// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameModeBase.h"
#include "TIMERUNCharacter.h"
#include "TIMERUNPlayerController.h"

ATIMERUNGameModeBase::ATIMERUNGameModeBase()
{	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_TIMERUNCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//DefaultPawnClass = ATIMERUNCharacter::StaticClass();

	PlayerControllerClass = ATIMERUNPlayerController::StaticClass();
}
 