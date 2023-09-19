// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "ISelectable.h"
#include "PlacementPreview.h"
#include "Input/PlayerInputActions.h"
#include "Net/UnrealNetwork.h"

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║												  ◆◆◆◆◆◆ Constructor ◆◆◆◆◆◆	 					                       ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
ARTSPlayerController::ARTSPlayerController(const FObjectInitializer& objectInitializer)
{
}
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║										         ◆◆◆◆◆◆ Unreal Events ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
void ARTSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARTSPlayerController, Selections, COND_OwnerOnly);
}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false);
	SetInputMode(inputMode);
	bShowMouseCursor = true;
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if(UEnhancedInputLocalPlayerSubsystem* inputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		inputSubSystem->ClearAllMappings();
		SetInputDefault();
	}
}

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Class Methods ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

void ARTSPlayerController::AddInputMapping(const UInputMappingContext* inputMapping, const int32 mappingPriority) const
{
	if(UEnhancedInputLocalPlayerSubsystem* inputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(inputMapping);
		if(!inputSubSystem->HasMappingContext(inputMapping))
		{
			inputSubSystem->AddMappingContext(inputMapping, mappingPriority);
		}
	}
}

void ARTSPlayerController::RemoveInputMapping(const UInputMappingContext* inputMapping) const
{
	if(UEnhancedInputLocalPlayerSubsystem* inputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(inputMapping);
		inputSubSystem->RemoveMappingContext(inputMapping);
	}
}

void ARTSPlayerController::SetInputDefault(const bool enabled) const
{
	ensureMsgf(PlayerActionAsset, TEXT("PlayerActionAsset is NULL !"));

	if(const UPlayerInputActions* playerActions = Cast<UPlayerInputActions>(PlayerActionAsset))
	{
		ensure(playerActions->MapPriorityCameraInput);
		
		if(enabled)
		{
			AddInputMapping(playerActions->PlayerInputMappingContext, playerActions->MapPriorityCameraInput);
		}
		else
		{
			RemoveInputMapping(playerActions->PlayerInputMappingContext);
		}
	}
}

void ARTSPlayerController::SetInputPlacement(const bool enabled) const
{
	if(const UPlayerInputActions* playerActions = Cast<UPlayerInputActions>(PlayerActionAsset))
	{
		ensure(playerActions->PlacementMappingContext);
		if(enabled)
		{
			AddInputMapping(playerActions->PlacementMappingContext, playerActions->MapPriorityPlacement);
			SetInputDefault(!enabled);
		}
		else
		{
			RemoveInputMapping(playerActions->PlacementMappingContext);
			SetInputDefault();
		}
	}
}


void ARTSPlayerController::HandleSelection(AActor* actorToSelect)
{
	if(Cast<ISelectable>(actorToSelect))
	{
		ActorSelected(actorToSelect) ? ServerDeSelect(actorToSelect) : ServerSelect(actorToSelect);
	}
	else
	{
		ServerClearSelected();
	}
}

void ARTSPlayerController::HandleSelection(TArray<AActor*> actorsToSelect)
{
	ServerSelectGroup(actorsToSelect);
}

FVector ARTSPlayerController::GetMousePositionOnTerrain() const
{
	FVector worldLocation, worldDirection;
	DeprojectMousePositionToWorld(worldLocation, worldDirection);
	
	const FVector end = worldLocation + worldDirection * 1000000.0f;
	if(FHitResult hit; GetWorld()->LineTraceSingleByChannel(hit, worldLocation, end, ECC_GameTraceChannel1))
	{
		if(hit.bBlockingHit)
		{
			return hit.Location;
		}
	}
	return FVector::ZeroVector;
}

FVector ARTSPlayerController::GetMousePositionOnSurface() const
{
	FVector worldLocation, worldDirection;
	DeprojectMousePositionToWorld(worldLocation, worldDirection);
	
	const FVector end = worldLocation + worldDirection * 1000000.0f;
	if(FHitResult hit; GetWorld()->LineTraceSingleByChannel(hit, worldLocation, end, ECC_Visibility))
	{
		if(hit.bBlockingHit)
		{
			return hit.Location;
		}
	}
	return FVector::ZeroVector;
}

bool ARTSPlayerController::ActorSelected(AActor* actorToCheck) const
{
	return actorToCheck && Selections.Contains(actorToCheck);
}

void ARTSPlayerController::ServerSelect_Implementation(AActor* actorToSelect)
{
	ServerClearSelected();
	if(ISelectable* selectable = Cast<ISelectable>(actorToSelect))
	{
		selectable->Select();
		Selections.Add(actorToSelect);
		OnRepSelected();
	}
}

void ARTSPlayerController::ServerSelectGroup_Implementation(const TArray<AActor*>& actorsToSelect)
{
	ServerClearSelected();
	TArray<AActor*> validActors;
	for(AActor* actorToSelect : actorsToSelect)
	{
		if(!actorToSelect) continue;
		if(ISelectable* selectable = Cast<ISelectable>(actorToSelect))
		{
			validActors.Add(actorToSelect);
			selectable->Select();
		}
	}
	Selections.Append(validActors);
	OnRepSelected();
	validActors.Empty();
}

void ARTSPlayerController::ServerDeSelect_Implementation(AActor* actorToDeSelect)
{
	if(ISelectable* selectable = Cast<ISelectable>(actorToDeSelect))
	{
		selectable->DeSelect();
		Selections.Remove(actorToDeSelect);
		OnRepSelected();
	}
}

void ARTSPlayerController::ServerClearSelected_Implementation()
{
	for (AActor* selection : Selections)
	{
		ISelectable* selectable = Cast<ISelectable>(selection);
		if(!selectable) continue;
		selectable->DeSelect();
	}
	Selections.Empty();
}

void ARTSPlayerController::OnRepSelected()
{
	OnSelectionUpdated.Broadcast();
}

void ARTSPlayerController::SetPlacementPreview()
{
	if(PreviewActorType && !bPlacementModeEnabled)
	{
		FTransform spawnTransform;
		spawnTransform.SetLocation(GetMousePositionOnSurface());
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PlacementPreviewActor = GetWorld()->SpawnActor<APlacementPreview>(PreviewActorType, spawnTransform, spawnParams);

		if(PlacementPreviewActor)
		{
			//SetInputPlacement();
			bPlacementModeEnabled = true;
		}
	}
}

void ARTSPlayerController::Place()
{
	if(!IsPlacementModeEnabled() || !PlacementPreviewActor) return;
	bPlacementModeEnabled = false;
	SetInputPlacement(false);
	ServerPlace(PlacementPreviewActor);
}

void ARTSPlayerController::PlaceCancel()
{
}

void ARTSPlayerController::ServerPlace_Implementation(AActor* placementPreviewToSpawn)
{
}
