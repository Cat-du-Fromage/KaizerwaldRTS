// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RTSPlayerController.h"
#include "SelectionBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/PlayerInputActions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║												  ◆◆◆◆◆◆ Constructor ◆◆◆◆◆◆	 					                       ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
ASPlayerPawn::ASPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ Scene Component Initialization ◇◇◇◇◇◇				                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ Spring Arm Component Initialization ◇◇◇◇◇◇                                                             │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.0f;
	SpringArmComponent->bDoCollisionTest = false;

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ Camera Component Initialization ◇◇◇◇◇◇				                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	/*
	MoveSpeed = 20.0f;
	EdgeScrollSpeed = 3.0f;
	RotateSpeed = 2.0f;
	RotatePitchMin = 10.0f;
	RotatePitchMax = 80.0f;
	ZoomSpeed = 200.0f;
	MinZoom = 500.0f;
	MaxZoom = 4000.0f;
	Smoothing = 2.0f
	*/
}
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║										         ◆◆◆◆◆◆ Unreal Events ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
// Called when the game starts or when spawned
void ASPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//Set an initial desired zoom and location
	TargetLocation = GetActorLocation();
	TargetZoom = 3000.0f;

	//Set an initial rotation for the camera
	const FRotator rotation = SpringArmComponent->GetRelativeRotation();
	TargetRotation = FRotator(rotation.Pitch - 50, rotation.Yaw, 0.0f);

	// Assign Player Controller Reference
	RTSPlayerController = Cast<ARTSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	CreateSelectionBox();
}

// Called every frame
void ASPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraBounds();
	EdgeScroll();
	
	//move camera in the desired location
	const FVector interpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, Smoothing);
	SetActorLocation(interpolatedLocation);

	//Zoom camera in the desired direction
	const float interpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, Smoothing);
	SpringArmComponent->TargetArmLength = interpolatedZoom;

	const FRotator interpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, Smoothing);
	SpringArmComponent->SetRelativeRotation(interpolatedRotation);
}

// Called to bind functionality to input
void ASPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//CastChecked does not check if cast succed
	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	const ARTSPlayerController* playerController = Cast<ARTSPlayerController>(GetController());

	if(IsValid(input) && IsValid(playerController))
	{
		if(const UPlayerInputActions* playerActions = Cast<UPlayerInputActions>(playerController->GetInputActionsAsset()))
		{
			EPlayerInputActions::BindInput_TriggerOnly(input, playerActions->Move, this, &ASPlayerPawn::Move);
			EPlayerInputActions::BindInput_TriggerOnly(input, playerActions->Zoom, this, &ASPlayerPawn::Zoom);
			EPlayerInputActions::BindInput_TriggerOnly(input, playerActions->Look, this, &ASPlayerPawn::Look);
			EPlayerInputActions::BindInput_TriggerOnly(input, playerActions->Rotate, this, &ASPlayerPawn::Rotate);
			EPlayerInputActions::BindInput_StartTriggerComplete(input, playerActions->Select, this,&ASPlayerPawn::Select, &ASPlayerPawn::SelectHold, &ASPlayerPawn::SelectEnd);
		}
	}
}

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Class Methods ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

void ASPlayerPawn::GetTerrainPosition(FVector& terrainPosition)
{
	if(UWorld* worldContext = GetWorld())
	{
		const FVector offset = FVector(0,0,10000);//Z is the UP vector!!
		FVector traceOrigin = terrainPosition + offset;
		FVector traceEnd = terrainPosition - offset;
		
		FCollisionQueryParams collisionParams;
		if(FHitResult hit; worldContext->LineTraceSingleByChannel(hit, traceOrigin, traceEnd, ECC_Visibility, collisionParams))
		{
			terrainPosition = hit.ImpactPoint;
		}
	}
}

void ASPlayerPawn::Move(const FInputActionValue& value)
{
	if(!SpringArmComponent) return;
	if(ensure(value.GetValueType() == EInputActionValueType::Axis2D))
	{
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(value.Get<FVector>()) * MoveSpeed;
		GetTerrainPosition(TargetLocation);
	}
}

void ASPlayerPawn::Zoom(const FInputActionValue& value)
{
	if(ensure(value.GetValueType() == EInputActionValueType::Axis1D))
	{
		TargetZoom = FMath::Clamp(TargetZoom + value.Get<float>() * ZoomSpeed, MinZoom, MaxZoom);
	}
}

void ASPlayerPawn::Look(const FInputActionValue& value)
{
	if(ensure(value.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float newPitch = value.Get<float>() * RotateSpeed * 0.5f;
		TargetRotation = FRotator(TargetRotation.Pitch + newPitch, TargetRotation.Yaw,0);
	}
}

void ASPlayerPawn::Rotate(const FInputActionValue& value)
{
	if(ensure(value.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float newRotation = value.Get<float>() * RotateSpeed; //Reminder: value negate
		TargetRotation = FRotator(TargetRotation.Pitch, TargetRotation.Yaw + newRotation,0);
	}
}

void ASPlayerPawn::EdgeScroll()
{
	if(UWorld* worldContext = GetWorld())
	{
		const FVector2D viewportSize = UWidgetLayoutLibrary::GetViewportSize(worldContext);
		FVector2D mousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(worldContext);
		mousePosition *= UWidgetLayoutLibrary::GetViewportScale(worldContext);
		mousePosition /= viewportSize;
		
		//Right/Left
		constexpr float highLimit = 0.98f;
		constexpr float lowLimit = 0.02f;
		if(mousePosition.X > highLimit && mousePosition.X < 1)
		{
			//NOTE Same as: TargetLocation += SpringArmComponent->GetRightVector() * EdgeScrollSpeed;
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector::RightVector) * EdgeScrollSpeed;
		}
		else if(mousePosition.X < lowLimit && mousePosition.X > 0)
		{
			//NOTE Same as: TargetLocation += SpringArmComponent->GetRightVector() * -EdgeScrollSpeed;
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector::LeftVector) * EdgeScrollSpeed;
		}

		//Backward/Forward
		if(mousePosition.Y > highLimit && mousePosition.Y < 1)
		{
			//NOTE same as : TargetLocation += SpringArmComponent->GetForwardVector() * -EdgeScrollSpeed;
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector::BackwardVector) * EdgeScrollSpeed;
		}
		else if(mousePosition.Y < lowLimit && mousePosition.Y > 0)
		{
			//NOTE same as : TargetLocation += SpringArmComponent->GetForwardVector() * EdgeScrollSpeed;
			TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector::ForwardVector) * EdgeScrollSpeed;
		}
		GetTerrainPosition(TargetLocation);
	}
}



void ASPlayerPawn::CameraBounds()
{
	float newPitch = TargetRotation.Pitch;
	if(TargetRotation.Pitch < -RotatePitchMax)
	{
		newPitch = -RotatePitchMax;
	}
	else if(TargetRotation.Pitch > -RotatePitchMin)
	{
		newPitch = -RotatePitchMin;
	}
	TargetRotation = FRotator(newPitch, TargetRotation.Yaw, 0.0f);
}

/** Mouse Inputs **/
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											  ◆◆◆◆◆◆ Mouse Inputs ◆◆◆◆◆◆	 					                       ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
AActor* ASPlayerPawn::GetSelectedObject()
{
	if(UWorld* world = GetWorld())
	{
		FVector worldLocation, worldDirection;
		RTSPlayerController->DeprojectMousePositionToWorld(worldLocation, worldDirection);
		
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		FVector end = worldLocation + worldDirection * 1000000.0f;
		if(FHitResult hit; world->LineTraceSingleByChannel(hit, worldLocation, end, ECC_Visibility, params))
		{
			return hit.GetActor();
			//if(AActor* hitActor = hit.GetActor()){return hitActor;}
		}
	}
	return nullptr;
}

void ASPlayerPawn::Select(const FInputActionValue& value)
{
	if(!RTSPlayerController) return;
	BoxSelectionEnabled = false;
	RTSPlayerController->HandleSelection(nullptr);
	SelectHitLocation = RTSPlayerController->GetMousePositionOnTerrain();
}

void ASPlayerPawn::SelectHold(const FInputActionValue& value)
{
	if(!RTSPlayerController) return;
	const float mouseDistance = (SelectHitLocation - RTSPlayerController->GetMousePositionOnTerrain()).Length();
	if(mouseDistance < 100) return;
	
	if(!BoxSelectionEnabled && SelectionBox)
	{
		SelectionBox->Start(SelectHitLocation, TargetRotation);
		BoxSelectionEnabled = true;
	}
}

void ASPlayerPawn::SelectEnd(const FInputActionValue& value)
{
	if(!RTSPlayerController) return;
	if(BoxSelectionEnabled && SelectionBox)
	{
		SelectionBox->End();
		BoxSelectionEnabled = false;
	}
	else
	{
		RTSPlayerController->HandleSelection(GetSelectedObject());
	}
}

void ASPlayerPawn::TestPlacement(const FInputActionValue& value)
{
	if(!RTSPlayerController) return;
	//RTSPlayerController->SetPlacementPreview();
}

void ASPlayerPawn::Place(const FInputActionValue& value)
{
	if(!RTSPlayerController) return;
	//if(RTSPlayerController->IsPlacementModeEnabled()) RTSPlayerController->Place();
}

void ASPlayerPawn::PlaceCancel(const FInputActionValue& value)
{
	if(!RTSPlayerController) return;
	//if(RTSPlayerController->IsPlacementModeEnabled()) RTSPlayerController->PlaceCancel();
}


void ASPlayerPawn::CreateSelectionBox()
{
	if(!SelectionBoxClass) return;
	if(UWorld* worldContext = GetWorld())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = this;
		spawnParams.Owner = this;
		SelectionBox = worldContext->SpawnActor<ASelectionBox>(SelectionBoxClass, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
		if(SelectionBox)
		{
			SelectionBox->SetOwner(this);
		}
	}
}



