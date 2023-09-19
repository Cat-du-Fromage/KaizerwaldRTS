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

	/** Placement Mapping Context **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputMappingContext* PlacementMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	int32 MapPriorityPlacement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* Place;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerInput")
	UInputAction* PlaceCancel;
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
}