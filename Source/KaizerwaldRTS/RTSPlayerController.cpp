// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "ISelectable.h"
#include "KaizerwaldRTSCharacter.h"
#include "PlacementPreview.h"
#include "Input/PlayerInputActions.h"
#include "Net/UnrealNetwork.h"

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║												  ◆◆◆◆◆◆ Constructor ◆◆◆◆◆◆	 					                       ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
ARTSPlayerController::ARTSPlayerController(const FObjectInitializer& objectInitializer)
{
	bPlacementModeEnabled = false;
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

void ARTSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!bPlacementModeEnabled || !PlacementPreviewActor) return;
	UpdatePlacement();
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

void ARTSPlayerController::SetInputShift(const bool enabled) const
{
	ensureMsgf(PlayerActionAsset, TEXT("PlayerActionAsset is NULL !"));
	if(const UPlayerInputActions* playerActions = Cast<UPlayerInputActions>(PlayerActionAsset))
	{
		ensure(playerActions->MappingContextShift);
		if(enabled)
		{
			AddInputMapping(playerActions->MappingContextShift, playerActions->MapPriorityShift);
			//SetInputDefault(!enabled);
		}
		else
		{
			RemoveInputMapping(playerActions->MappingContextShift);
			//SetInputDefault();
		}
	}
}

void ARTSPlayerController::SetInputAlt(const bool enabled) const
{
	ensureMsgf(PlayerActionAsset, TEXT("PlayerActionAsset is NULL !"));
	if(const UPlayerInputActions* playerActions = Cast<UPlayerInputActions>(PlayerActionAsset))
	{
		ensure(playerActions->MappingContextAlt);
		if(enabled)
		{
			AddInputMapping(playerActions->MappingContextAlt, playerActions->MapPriorityAlt);
			//SetInputDefault(!enabled);
		}
		else
		{
			RemoveInputMapping(playerActions->MappingContextAlt);
			//SetInputDefault();
		}
	}
}

void ARTSPlayerController::SetInputCtrl(const bool enabled) const
{
	ensureMsgf(PlayerActionAsset, TEXT("PlayerActionAsset is NULL !"));
	if(const UPlayerInputActions* playerActions = Cast<UPlayerInputActions>(PlayerActionAsset))
	{
		ensure(playerActions->MappingContextCtrl);
		if(enabled)
		{
			AddInputMapping(playerActions->MappingContextCtrl, playerActions->MapPriorityCtrl);
			//SetInputDefault(!enabled);
		}
		else
		{
			RemoveInputMapping(playerActions->MappingContextCtrl);
			//SetInputDefault();
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

void ARTSPlayerController::HandleDeSelection(AActor* actorToDeSelect)
{
	if(actorToDeSelect && ActorSelected(actorToDeSelect))
	{
		ServerDeSelect(actorToDeSelect);
	}
}

void ARTSPlayerController::HandleDeSelection(TArray<AActor*> actorsToDeSelect)
{
	ServerDeSelectGroup(actorsToDeSelect);
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
	//ServerClearSelected();
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

void ARTSPlayerController::ServerDeSelectGroup_Implementation(const TArray<AActor*>& actorsToDeSelect)
{
	for(AActor* actorToDeselect : actorsToDeSelect)
	{
		if(!actorToDeselect) continue;
		for(int32 i = Selections.Num() - 1; i >= 0; --i)
		{
			if(actorToDeselect != Selections[i]) continue;
			if(ISelectable* selectable = Cast<ISelectable>(actorToDeselect))
			{
				selectable->DeSelect();
				Selections.RemoveAt(i);
				break;
			}
		}
	}
	OnRepSelected();
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
	OnRepSelected();
}

void ARTSPlayerController::OnRepSelected()
{
	OnSelectionUpdated.Broadcast();
}

void ARTSPlayerController::UpdatePlacement() const
{
	if(!PlacementPreviewActor) return;
	PlacementPreviewActor->SetActorLocation(GetMousePositionOnSurface());
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
			SetInputPlacement();
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
	if(!IsPlacementModeEnabled() || !PlacementPreviewActor) return;
	bPlacementModeEnabled = false;
	SetInputPlacement(false);
	EndPlacement();
}

void ARTSPlayerController::ServerPlace_Implementation(AActor* placementPreviewToSpawn)
{
	if(const APlacementPreview* preview = Cast<APlacementPreview>(placementPreviewToSpawn))
	{
		FTransform spawnTransform;
		const FVector location = preview->GetActorLocation();
		spawnTransform.SetLocation(location + FVector::UpVector * 100.0f);
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if(AKaizerwaldRTSCharacter* newUnit = GetWorld()->SpawnActor<AKaizerwaldRTSCharacter>(preview->PlaceableClass, spawnTransform, spawnParams))
		{
			newUnit->SetOwner(this);
		}
	}
	EndPlacement();
}

void ARTSPlayerController::EndPlacement_Implementation()
{
	PlacementPreviewActor->Destroy();
}
