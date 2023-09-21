// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "SPlayerPawn.generated.h"

class ASelectionBox;
class ARTSPlayerController;
class UInputMappingContext;
//do this instead of include when you only need to declare pointers, references,
//or function signatures involving the class without needing its full definition. 
class UCameraComponent;
class USpringArmComponent;
UCLASS()
class KAIZERWALDRTS_API ASPlayerPawn : public APawn
{
	GENERATED_BODY()

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Properties ◆◆◆◆◆◆			                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	//UInputMappingContext* PlayerCameraMappingContext;
	//UEnhancedInputComponent* PlayerEnhancedInputComponent;
	
	
protected:
	UPROPERTY()
	ARTSPlayerController* RTSPlayerController; //SPlayer (In Tutorial)
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float MoveSpeed = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float EdgeScrollSpeed = 30.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float RotateSpeed = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float RotatePitchMin = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float RotatePitchMax = 80.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float ZoomSpeed = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float MinZoom = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float MaxZoom = 4000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float Smoothing = 2.0f;
	
	/** Box selection **/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Mouse")
	TSubclassOf<ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* SelectionBox;

	//Move to SelectionBox?
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Settings|Mouse")
	bool BoxSelectionEnabled; //BoxSelected(in tuto)

	//Move to SelectionBox?
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Settings|Mouse")
	FVector SelectHitLocation; //In Unity was -> MouseStart
	
private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	float TargetZoom;
	
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											       ◆◆◆◆◆◆ METHODS ◆◆◆◆◆◆			                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
public:
	// Sets default values for this pawn's properties
	ASPlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void GetTerrainPosition(FVector& terrainPosition);
	//╓────────────────────────────────────────────────────────────────────────────────────────────────────────────────╖
	//║ ◈◈◈◈◈◈ Movement Methods ◈◈◈◈◈◈					                                                           ║
	//╙────────────────────────────────────────────────────────────────────────────────────────────────────────────────╜
	UFUNCTION()
	void EdgeScroll();

	//Mouse Inputs

	UFUNCTION()
	AActor* GetSelectedObject();
	
	//Move so selection box?
	UFUNCTION()
	void CreateSelectionBox();
	
	//ENHANCED INPUTS
	UFUNCTION()
	void Move(const FInputActionValue& inputValue);

	UFUNCTION()
	void Zoom(const FInputActionValue& inputValue);

	UFUNCTION()
	void Look(const FInputActionValue& inputValue);
	
	UFUNCTION()
	void Rotate(const FInputActionValue& inputValue);

	UFUNCTION()
	void Select(const FInputActionValue& inputValue);

	UFUNCTION()
	void SelectHold(const FInputActionValue& inputValue);

	UFUNCTION()
	void SelectEnd(const FInputActionValue& inputValue);

	UFUNCTION()
	void TestPlacement(const FInputActionValue& inputValue);

	UFUNCTION()
	void SelectDoubleTap(const FInputActionValue& inputValue);

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ MODIFIER KEYS ◇◇◇◇◇◇								                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘

	UFUNCTION()
	void Shift(const FInputActionValue& inputValue);

	UFUNCTION()
	void Alt(const FInputActionValue& inputValue);

	UFUNCTION()
	void Ctrl(const FInputActionValue& inputValue);

	//╓────────────────────────────────────────────────────────────────────────────────────────────────────────────────╖
	//║ ◈◈◈◈◈◈ Placement Methods ◈◈◈◈◈◈				                                                           ║
	//╙────────────────────────────────────────────────────────────────────────────────────────────────────────────────╜

	UFUNCTION()
	void Place(const FInputActionValue& inputValue);

	UFUNCTION()
	void PlaceCancel(const FInputActionValue& inputValue);

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ SHIFT ◇◇◇◇◇◇										                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UFUNCTION()
	void ShiftSelect(const FInputActionValue& inputValue);

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ ALT ◇◇◇◇◇◇											                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UFUNCTION()
	void AltSelect(const FInputActionValue& inputValue);
	
	//void AltSelectHold(const FInputActionValue& inputValue);

	UFUNCTION()
	void AltSelectEnd(const FInputActionValue& inputValue);

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ CTRL ◇◇◇◇◇◇											                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	UFUNCTION()
	void CtrlSelect(const FInputActionValue& inputValue);

	UFUNCTION()
	void CtrlSelectEnd(const FInputActionValue& inputValue);
	
private:
	UFUNCTION()
	void CameraBounds();
};
