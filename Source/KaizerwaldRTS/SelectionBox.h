// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

class ARTSPlayerController;
class UBoxComponent;

UCLASS()
class KAIZERWALDRTS_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASelectionBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Start(FVector position, const FRotator rotation);

	UFUNCTION()
	void End();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Adjust() const;

	UFUNCTION()
	void Manage();

	UFUNCTION()
	void HandleHighlight(AActor* actorInBox, const bool highlight = true) const;

	UFUNCTION()
	void OnBoxColliderBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);
	
	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FRotator StartRotation;

	UPROPERTY()
	TArray<AActor*> ActorsInBox;

	UPROPERTY()
	TArray<AActor*> CenterInBox;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;

	UPROPERTY()
	bool IsEnabled; //BoxSelect (In Tuto)

	UPROPERTY()
	ARTSPlayerController* PlayerController;
};
