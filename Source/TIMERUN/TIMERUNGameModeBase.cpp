// Fill out your copyright notice in the Description page of Project Settings.


#include "TIMERUNGameModeBase.h"
#include "TIMERUNCharacter.h"


ATIMERUNGameModeBase::ATIMERUNGameModeBase()
{	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_TIMERUNCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//DefaultPawnClass = ATIMERUNCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(TEXT("/Game/Player/Control/BP_TIMERUNController"));
	//static ConstructorHelpers::FClassFinder<AController> PlayerControllerBPClass(TEXT("/Game/Player/Control/BP_TestController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	//PlayerControllerClass = ATIMERUNPlayerController::StaticClass();

}
 