// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	float EdgeScrollSpeed = 3.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float RotateSpeed = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float RotatePitchMin = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float RotatePitchMax = 80.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float ZoomSpeed = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float MinZoom = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Camera")
	float MaxZoom = 4000.0f;

	/** Box selection **/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Mouse")
	float LeftMouseHoldThreshold = 0.15f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings|Mouse")
	TSubclassOf<ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* SelectionBox;

	//Move to SelectionBox?
	UPROPERTY()
	bool BoxSelectionEnabled; //BoxSelected(in tuto)

	//Move to SelectionBox?
	UPROPERTY()
	FVector LeftMouseHitLocation; //In Unity was -> MouseStart
	
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

	UPROPERTY()
	bool CanRotate;
	
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											       ◆◆◆◆◆◆ Methods ◆◆◆◆◆◆			                                   ║
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
	void Forward(float AxisValue);

	UFUNCTION()
	void Right(float AxisValue);

	UFUNCTION()
	void Zoom(float AxisValue);

	UFUNCTION()
	void RotateHorizontal(float AxisValue);

	UFUNCTION()
	void RotateVertical(float AxisValue);

	UFUNCTION()
	void RotateRight();

	UFUNCTION()
	void RotateLeft();

	UFUNCTION()
	void EnableRotate();

	UFUNCTION()
	void DisableRotate();

	UFUNCTION()
	void EdgeScroll();

	//Mouse Inputs

	UFUNCTION()
	AActor* GetSelectedObject();

	UFUNCTION()
	void MouseLeftPressed();

	UFUNCTION()
	void MouseLeftReleased();

	UFUNCTION()
	void LeftMouseInputHeld(float axisValue);

	UFUNCTION()
	void MouseRightPressed();

	UFUNCTION()
	void MouseRightReleased();

	//Move so selection box?
	UFUNCTION()
	void CreateSelectionBox();
	
	//ENHANCED INPUTS
	//void OnMovement();
	
private:
	UFUNCTION()
	void CameraBounds();
};
