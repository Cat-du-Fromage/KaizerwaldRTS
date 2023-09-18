// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectionBox.h"

#include "ISelectable.h"
#include "RTSPlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASelectionBox::ASelectionBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init BoxCollider
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetBoxExtent(FVector::OneVector);
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxCollider;
	
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ASelectionBox::OnBoxColliderBeginOverlap);
	
	//Init DecalComponent
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetupAttachment(RootComponent);
	
	IsEnabled = false;
}

// Called when the game starts or when spawned
void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(false);
	if(DecalComponent)
	{
		DecalComponent->SetVisibility(false);
	}
	PlayerController = Cast<ARTSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
}

// Called every frame
void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsEnabled)
	{
		Adjust();
		Manage();
	}
}

void ASelectionBox::Adjust() const
{
	if(!PlayerController || !BoxCollider || !DecalComponent) return;
	
	const FVector mousePositionOnTerrain =  PlayerController->GetMousePositionOnTerrain();
	const FVector endPoint = FVector(mousePositionOnTerrain.X, mousePositionOnTerrain.Y, 0);
	
	BoxCollider->SetWorldLocation(UKismetMathLibrary::VLerp(StartLocation, endPoint, 0.5f));
	
	FVector newExtend = FVector(GetActorLocation().X,GetActorLocation().Y,0) - endPoint;
	newExtend = GetActorRotation().GetInverse().RotateVector(newExtend);
	newExtend = newExtend.GetAbs();
	newExtend.Z += 100000.0f;
	BoxCollider->SetBoxExtent(newExtend);

	//we have to change the order of axis because the box is rotated 90° so Z is now forward, y right, x down etc..
	FVector decalSize = FVector(newExtend.Z,newExtend.Y,newExtend.X);
	DecalComponent->DecalSize = decalSize;
}

void ASelectionBox::Manage()
{
	if(!BoxCollider) return;
	for(int32 i = 0; i < ActorsInBox.Num(); ++i)
	{
		AActor* currentActor = ActorsInBox[i];
		if(!currentActor) continue;
		//Get actor center and transform into local space of the collider
		FVector actorCenter = currentActor->GetActorLocation();
		const FVector localActorCenter = BoxCollider->GetComponentTransform().InverseTransformPosition(actorCenter);

		//Get the local extents of the collider
		const FVector localExtents = BoxCollider->GetUnscaledBoxExtent();

		//AABB
		const bool xInBounds = localActorCenter.X >= -localExtents.X && localActorCenter.X <= localExtents.X;
		const bool yInBounds = localActorCenter.Y >= -localExtents.Y && localActorCenter.Y <= localExtents.Y;
		const bool zInBounds = localActorCenter.Z >= -localExtents.Z && localActorCenter.Z <= localExtents.Z;
		if(xInBounds && yInBounds && zInBounds)
		{
			if(CenterInBox.Contains(currentActor)) continue;
			CenterInBox.Add(currentActor);
			HandleHighlight(currentActor, true);
		}
		else
		{
			if(!CenterInBox.Contains(currentActor)) continue;
			CenterInBox.Remove(currentActor);
			HandleHighlight(currentActor, false);
		}
	}
}

void ASelectionBox::HandleHighlight(AActor* actorInBox, const bool highlight) const
{
	if(ISelectable* selectable = Cast<ISelectable>(actorInBox))
	{
		selectable->SetHighlight(highlight);
	}
}

void ASelectionBox::OnBoxColliderBeginOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if(!otherActor || otherActor == this) return;
	if(ISelectable* selectable = Cast<ISelectable>(otherActor))
	{
		ActorsInBox.AddUnique(otherActor);
	}
}

void ASelectionBox::Start(FVector position, const FRotator rotation)
{
	if(!DecalComponent) return;
	StartLocation = FVector(position.X, position.Y, 0);
	StartRotation = FRotator(0, rotation.Yaw, 0);

	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	SetActorEnableCollision(true);

	DecalComponent->SetVisibility(true);
	ActorsInBox.Empty();
	CenterInBox.Empty();
	IsEnabled = true;
}

void ASelectionBox::End()
{
	if(!PlayerController) return;
	IsEnabled = false;
	SetActorEnableCollision(false);
	DecalComponent->SetVisibility(false);

	if(CenterInBox.Num() == 0)
	{
		PlayerController->HandleSelection(nullptr);
	}
	else
	{
		PlayerController->HandleSelection(CenterInBox);
	}

	ActorsInBox.Empty();
	CenterInBox.Empty();
}

