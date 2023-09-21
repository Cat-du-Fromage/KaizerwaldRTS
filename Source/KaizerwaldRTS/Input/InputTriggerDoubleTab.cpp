// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTriggerDoubleTab.h"
#include "EnhancedPlayerInput.h"

ETriggerState UInputTriggerDoubleTab::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,FInputActionValue ModifiedValue, float DeltaTime)
{
	if(IsActuated(ModifiedValue) && !IsActuated(LastValue))
	{
		const float currentTime = PlayerInput->GetOuterAPlayerController()->GetWorld()->GetRealTimeSeconds();
		if(currentTime - LastTapedTime < Delay)
		{
			LastTapedTime = 0;
			return ETriggerState::Triggered;
		}
		LastTapedTime = currentTime;
	}
	return ETriggerState::None;
	//return Super::UpdateState_Implementation(PlayerInput, ModifiedValue, DeltaTime);
}
