// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Engine/DataAsset.h"
#include "PlayerInputActions.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS(BlueprintType)
class KAIZERWALDRTS_API UPlayerInputActions : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputMappingContext* PlayerInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	int32 MapPriorityCameraInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* Rotate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* Zoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* Select;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* TestPlacement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* SelectDoubleTap;

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ MODIFIER KEYS ◇◇◇◇◇◇								                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Modifier")
	UInputAction* Shift;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Modifier")
	UInputAction* Alt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Modifier")
	UInputAction* Ctrl;

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ PLACEMENT MAPPING CONTEXT ◇◇◇◇◇◇							                                           │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlacementInput")
	UInputMappingContext* PlacementMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlacementInput")
	int32 MapPriorityPlacement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlacementInput")
	UInputAction* Place;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlacementInput")
	UInputAction* PlaceCancel;

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ SHIFT MAPPING CONTEXT ◇◇◇◇◇◇								                                           │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shift")
	UInputMappingContext* MappingContextShift;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shift")
	int32 MapPriorityShift;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shift")
	UInputAction* ShiftSelect;
	
	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ ALT MAPPING CONTEXT ◇◇◇◇◇◇									                                           │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Alt")
	UInputMappingContext* MappingContextAlt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Alt")
	int32 MapPriorityAlt;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Alt")
	UInputAction* AltSelect;
	
	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ CTRL MAPPING CONTEXT ◇◇◇◇◇◇									                                           │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ctrl")
	UInputMappingContext* MappingContextCtrl;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ctrl")
	int32 MapPriorityCtrl;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ctrl")
	UInputAction* CtrlSelect;
};

namespace EPlayerInputActions
{
	template<class T, class FuncType>
	void BindInput_TriggerOnly(UEnhancedInputComponent* Input, const UInputAction* Action, T* Object, FuncType TriggerFunc)
	{
		if(TriggerFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Triggered, Object, TriggerFunc);
		}
	}

	template<class T, class FuncType>
	void BindInput_StartTriggerComplete(UEnhancedInputComponent* Input, const UInputAction* Action, T* Object, FuncType StartFunc, FuncType TriggerFunc, FuncType CompleteFunc)
	{
		if(StartFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Started, Object, StartFunc);
		}
		if(TriggerFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Triggered, Object, TriggerFunc);
		}
		if(CompleteFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Completed, Object, CompleteFunc);
		}
	}

	template<class T, class FuncType>
	void BindInput_Simple(UEnhancedInputComponent* Input, const UInputAction* Action, T* Object, FuncType TriggerFunc, FuncType CompleteFunc)
	{
		if(TriggerFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Started, Object, TriggerFunc); //trigger once(Started)
		}
		if(CompleteFunc != nullptr)
		{
			Input->BindAction(Action, ETriggerEvent::Completed, Object, CompleteFunc);
		}
	}
}