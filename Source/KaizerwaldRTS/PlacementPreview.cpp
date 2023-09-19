// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacementPreview.h"


//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║												  ◆◆◆◆◆◆ Constructor ◆◆◆◆◆◆	 					                       ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
APlacementPreview::APlacementPreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ Scene Component Initialization ◇◇◇◇◇◇				                                                   │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;
	
	//┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	//│  ◇◇◇◇◇◇ StaticMesh Initialization ◇◇◇◇◇◇											                           │
	//└────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	bReplicates = false; //Multiplayer
	SetActorEnableCollision(false);
}

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║										         ◆◆◆◆◆◆ Unreal Events ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
// Called when the game starts or when spawned
void APlacementPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlacementPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
//║											     ◆◆◆◆◆◆ Class Methods ◆◆◆◆◆◆		                                   ║
//╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
