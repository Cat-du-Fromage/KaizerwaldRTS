// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlacementPreview.generated.h"

class AKaizerwaldRTSCharacter;
UCLASS()
class KAIZERWALDRTS_API APlacementPreview : public AActor
{
	GENERATED_BODY()
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Properties ◆◆◆◆◆◆			                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement")
	TSubclassOf<AKaizerwaldRTSCharacter> PlaceableClass;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	
//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											       ◆◆◆◆◆◆ METHODS ◆◆◆◆◆◆			                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
	
public:	
	// Sets default values for this actor's properties
	APlacementPreview();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	

};
