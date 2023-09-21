// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

class UInputMappingContext;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectionUpdatedDelegate);

UCLASS()
class KAIZERWALDRTS_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Properties ◆◆◆◆◆◆			                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
public:
	
protected:
	UPROPERTY()
	FSelectionUpdatedDelegate OnSelectionUpdated;
	
	UPROPERTY(ReplicatedUsing = OnRepSelected)
	TArray<AActor*> Selections;
	
	//╓────────────────────────────────────────────────────────────────────────────────────────────────────────────────╖
	//║ ◈◈◈◈◈◈ Enhanced Inputs ◈◈◈◈◈◈																			   ║
	//╙────────────────────────────────────────────────────────────────────────────────────────────────────────────────╜
	//Equivalent de InputActionAssets dans Unity
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Settings")
	UDataAsset* PlayerActionAsset;

	//╓────────────────────────────────────────────────────────────────────────────────────────────────────────────────╖
	//║ ◈◈◈◈◈◈ Placement Methods ◈◈◈◈◈◈				                                                           ║
	//╙────────────────────────────────────────────────────────────────────────────────────────────────────────────────╜
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bPlacementModeEnabled;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AActor* PlacementPreviewActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Placeable")
	TSubclassOf<AActor> PreviewActorType;
	
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║												     ◆◆◆◆◆◆ METHODS ◆◆◆◆◆◆			                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
public:
	ARTSPlayerController(const FObjectInitializer& objectInitializer = FObjectInitializer::Get());
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void HandleSelection(AActor* actorToSelect);
	void HandleSelection(TArray<AActor*> actorsToSelect);

	UFUNCTION()
	void HandleDeSelection(AActor* actorToDeSelect);
	void HandleDeSelection(TArray<AActor*> actorsToDeSelect);

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;

	UFUNCTION()
	FVector GetMousePositionOnSurface() const;
	
	//╓────────────────────────────────────────────────────────────────────────────────────────────────────────────────╖
	//║ ◈◈◈◈◈◈ Enhanced Inputs ◈◈◈◈◈◈																			   ║
	//╙────────────────────────────────────────────────────────────────────────────────────────────────────────────────╜
	
	UFUNCTION()
	void AddInputMapping(const UInputMappingContext* inputMapping, const int32 mappingPriority = 0) const;

	UFUNCTION()
	void RemoveInputMapping(const UInputMappingContext* inputMapping) const;
	
	UFUNCTION()
	void SetInputDefault(const bool enabled = true) const;

	UFUNCTION()
	void SetInputPlacement(const bool enabled = true) const;
	
	UFUNCTION()
	void SetInputShift(const bool enabled = true) const;

	UFUNCTION()
	void SetInputAlt(const bool enabled = true) const;

	UFUNCTION()
	void SetInputCtrl(const bool enabled = true) const;

	UFUNCTION()
	UDataAsset* GetInputActionsAsset() const { return PlayerActionAsset; }

	//╓────────────────────────────────────────────────────────────────────────────────────────────────────────────────╖
	//║ ◈◈◈◈◈◈ Placement Methods ◈◈◈◈◈◈				                                                           ║
	//╙────────────────────────────────────────────────────────────────────────────────────────────────────────────────╜
	
	UFUNCTION()
	bool IsPlacementModeEnabled() const { return bPlacementModeEnabled; }
	
	UFUNCTION()
	void SetPlacementPreview();
	
	UFUNCTION()
	void Place();

	UFUNCTION()
	void PlaceCancel();
	
protected:
	
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	bool ActorSelected(AActor* actorToCheck) const;

	UFUNCTION(Server, Reliable)
	void ServerSelect(AActor* actorToSelect);

	UFUNCTION(Server, Reliable)
	void ServerSelectGroup(const TArray<AActor*>& actorsToSelect);

	UFUNCTION(Server, Reliable)
	void ServerDeSelect(AActor* actorToDeSelect);

	UFUNCTION(Server, Reliable)
	void ServerDeSelectGroup(const TArray<AActor*>& actorsToDeSelect);

	UFUNCTION(Server, Reliable)
	void ServerClearSelected();

	UFUNCTION()
	void OnRepSelected();
	
	/**Placement **/

	UFUNCTION()
	void UpdatePlacement() const;
	
	UFUNCTION(Server, Reliable)
	void ServerPlace(AActor* placementPreviewToSpawn);

	UFUNCTION(Client, Reliable)
	void EndPlacement();
};
