// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"

#include "ISelectable.h"
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

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Class Methods ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

void ARTSPlayerController::HandleSelection(AActor* actorToSelect)
{
	if(ISelectable* selectable = Cast<ISelectable>(actorToSelect))
	{
		if(actorToSelect && ActorSelected(actorToSelect))
		{
			ServerDeSelect(actorToSelect);
		}
		else
		{
			ServerSelect(actorToSelect);
		}
		//ActorSelected(actorToSelect) ? ServerDeSelect(actorToSelect) : ServerSelect(actorToSelect);
	}
	else
	{
		ServerClearSelected();
	}
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
