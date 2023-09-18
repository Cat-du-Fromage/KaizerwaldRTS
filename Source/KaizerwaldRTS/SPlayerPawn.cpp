// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerPawn.h"

#include "EnhancedInputSubsystems.h"
#include "RTSPlayerController.h"
#include "SelectionBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	const FVector interpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, MoveSpeed);
	SetActorLocation(interpolatedLocation);

	//Zoom camera in the desired direction
	const float interpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, MoveSpeed);
	SpringArmComponent->TargetArmLength = interpolatedZoom;

	const FRotator interpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	SpringArmComponent->SetRelativeRotation(interpolatedRotation);
}

// Called to bind functionality to input
void ASPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//void (ASPlayerPawn::*forwardFunction)(float) = &ASPlayerPawn::Forward;
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ASPlayerPawn::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ASPlayerPawn::Right);
	PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &ASPlayerPawn::Zoom);
	PlayerInputComponent->BindAxis(TEXT("RotateHorizontal"), this, &ASPlayerPawn::RotateHorizontal);
	PlayerInputComponent->BindAxis(TEXT("RotateVertical"), this, &ASPlayerPawn::RotateVertical);
	PlayerInputComponent->BindAxis(TEXT("MouseLeft"), this, &ASPlayerPawn::LeftMouseInputHeld);
	
	PlayerInputComponent->BindAction(TEXT("RotateRight"),IE_Pressed,this, &ASPlayerPawn::RotateRight);
	PlayerInputComponent->BindAction(TEXT("RotateLeft"),IE_Pressed,this, &ASPlayerPawn::RotateLeft);
	PlayerInputComponent->BindAction(TEXT("Rotate"),IE_Pressed,this, &ASPlayerPawn::EnableRotate);
	PlayerInputComponent->BindAction(TEXT("Rotate"),IE_Released,this, &ASPlayerPawn::DisableRotate);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"),IE_Pressed,this, &ASPlayerPawn::MouseLeftPressed);
	PlayerInputComponent->BindAction(TEXT("MouseLeft"),IE_Released,this, &ASPlayerPawn::MouseLeftReleased);
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

void ASPlayerPawn::Forward(float AxisValue)
{
	if(FMath::IsNearlyEqual(AxisValue, 0.0f)) return;
	TargetLocation = SpringArmComponent->GetForwardVector() * (AxisValue * MoveSpeed) + TargetLocation;
	GetTerrainPosition(TargetLocation);
}

void ASPlayerPawn::Right(float AxisValue)
{
	if(FMath::IsNearlyEqual(AxisValue, 0.0f)) return;
	TargetLocation = SpringArmComponent->GetRightVector() * (AxisValue * MoveSpeed) + TargetLocation;
	GetTerrainPosition(TargetLocation);
}

void ASPlayerPawn::Zoom(float AxisValue)
{
	if(FMath::IsNearlyEqual(AxisValue, 0.0f)) return;
	
	const float zoom = AxisValue * 100.f;
	TargetZoom = FMath::Clamp(zoom * TargetZoom, MinZoom, MaxZoom);
}

void ASPlayerPawn::RotateHorizontal(float AxisValue)
{
	if(FMath::IsNearlyEqual(AxisValue, 0.0f) || !CanRotate) return;

	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f,AxisValue,0.0f));
}

void ASPlayerPawn::RotateVertical(float AxisValue)
{
	if(FMath::IsNearlyEqual(AxisValue, 0.0f) || !CanRotate) return;

	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(AxisValue,0.0f,0.0f));
}

void ASPlayerPawn::RotateRight()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f,-45.0f,0.0f));
}

void ASPlayerPawn::RotateLeft()
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.0f,45.0f,0.0f));
}

void ASPlayerPawn::EnableRotate()
{
	CanRotate = true;
}

void ASPlayerPawn::DisableRotate()
{
	CanRotate = false;
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
		if(mousePosition.X > 0.98f && mousePosition.X < 1.0f)
		{
			Right(EdgeScrollSpeed);
		}
		else if(mousePosition.X < 0.02f && mousePosition.X > 0.0f)
		{
			Right(-EdgeScrollSpeed);
		}

		//Forward/backward
		if(mousePosition.Y > 0.98f && mousePosition.Y < 1.0f)
		{
			Forward(-EdgeScrollSpeed);
		}
		else if(mousePosition.Y < 0.02f && mousePosition.Y > 0.0f)
		{
			Forward(EdgeScrollSpeed);
		}
	}
}

/*
void ASPlayerPawn::OnMovement()
{
	
}
*/

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
	//TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, 0.0f);
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
			/*
			if(AActor* hitActor = hit.GetActor())
			{
				return hitActor;
			}
			*/
		}
	}
	return nullptr;
}

void ASPlayerPawn::MouseLeftPressed()
{
	if(!RTSPlayerController) return;
	BoxSelectionEnabled = false;
	RTSPlayerController->HandleSelection(nullptr);
	LeftMouseHitLocation = RTSPlayerController->GetMousePositionOnTerrain();
}

void ASPlayerPawn::MouseLeftReleased()
{
	if(!RTSPlayerController) return;
	//RTSPlayerController->HandleSelection(GetSelectedObject());
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

void ASPlayerPawn::LeftMouseInputHeld(float axisValue)
{
	if(!RTSPlayerController || FMath::IsNearlyEqual(axisValue, 0.0f)) return;
	if(RTSPlayerController->GetInputKeyTimeDown(EKeys::LeftMouseButton) < LeftMouseHoldThreshold) return;
	if(!BoxSelectionEnabled && SelectionBox)
	{
		SelectionBox->Start(LeftMouseHitLocation, TargetRotation);
		BoxSelectionEnabled = true;
	}
}

void ASPlayerPawn::MouseRightPressed()
{
}

void ASPlayerPawn::MouseRightReleased()
{
	if(!RTSPlayerController) return;
	//Box selection active?
	/*
	if(BoxSelectionEnabled && SelectionBox)
	{
		SelectionBox->End();
		BoxSelectionEnabled = false;
	}
	else
	{
		RTSPlayerController->HandleSelection(GetSelectedObject());
	}
	*/
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

