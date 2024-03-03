// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameModeBase.h"
#include "TIMERUNCharacter.h"

#include "TIMERUNPlayerController.h"

ATIMERUNGameModeBase::ATIMERUNGameModeBase()
{	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_TIMERUNCharacter_Ver2"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//DefaultPawnClass = ATIMERUNCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(TEXT("/Game/Player/BP_TIMERUNController_Ver2"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	//PlayerControllerClass = ATIMERUNPlayerController::StaticClass();

}
 