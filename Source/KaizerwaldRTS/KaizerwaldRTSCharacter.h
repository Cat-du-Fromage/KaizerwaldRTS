// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISelectable.h"
#include "GameFramework/Character.h"
#include "KaizerwaldRTSCharacter.generated.h"

/*
 *Utilisé dans le cadre d'un test de séléction
 */

UCLASS(Blueprintable)
class AKaizerwaldRTSCharacter : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	AKaizerwaldRTSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	// START ISelectable Interface
	virtual void Select() override;
	virtual void DeSelect() override;
	virtual void SetHighlight(const bool stateValue) override;
	// END ISelectable Interface

	UPROPERTY()
	bool IsSelected;
};

