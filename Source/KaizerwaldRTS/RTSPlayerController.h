// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectionUpdatedDelegate);
/**
 * 
 */
UCLASS()
class KAIZERWALDRTS_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARTSPlayerController(const FObjectInitializer& objectInitializer = FObjectInitializer::Get());
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void HandleSelection(AActor* actorToSelect);
	void HandleSelection(TArray<AActor*> actorsToSelect);

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;
	
protected:

	UPROPERTY()
	FSelectionUpdatedDelegate OnSelectionUpdated;
	
	UPROPERTY(ReplicatedUsing = OnRepSelected)
	TArray<AActor*> Selections;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	bool ActorSelected(AActor* actorToCheck) const;

	UFUNCTION(Server, Reliable)
	void ServerSelect(AActor* actorToSelect);

	UFUNCTION(Server, Reliable)
	void ServerSelectGroup(const TArray<AActor*>& actorsToSelect);

	UFUNCTION(Server, Reliable)
	void ServerDeSelect(AActor* actorToDeSelect);

	UFUNCTION(Server, Reliable)
	void ServerClearSelected();

	UFUNCTION()
	void OnRepSelected();
};
