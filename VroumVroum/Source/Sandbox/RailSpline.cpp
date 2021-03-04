#include "RailSpline.h"

#include "Components/SplineComponent.h"

ARailSpline::ARailSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	TopScene = CreateDefaultSubobject<USceneComponent>(TEXT("TopScene"));
	RootComponent = TopScene;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	Spline->SetupAttachment(RootComponent);

	CarStaticMeshType = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Assets/Meshes/Interactive/GOLD/SM_RC_Kart.SM_RC_Kart")).Object;
	LocomotiveStaticMeshType = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Assets/Meshes/Interactive/GOLD/SM_RC_Kart.SM_RC_Kart")).Object;

}

void ARailSpline::BeginPlay()
{
	Super::BeginPlay();

	SpawnMeshActor(0, LocomotiveStaticMeshType);

	for (int i = 0; i < CarNumber; i++)
	{
		SpawnMeshActor(i + 1, CarStaticMeshType);
	}
}

void ARailSpline::MoveMeshActor(const int CarIndex)
{
	const float CarDistanceOnSpline = GetCarSplineDistance(CarIndex);

	const FVector CarLocation = Spline->GetLocationAtDistanceAlongSpline(CarDistanceOnSpline, ESplineCoordinateSpace::World);
	const FRotator CarRotation = Spline->GetRotationAtDistanceAlongSpline(CarDistanceOnSpline, ESplineCoordinateSpace::World);

	CarActors[CarIndex].StaticMesh->SetWorldLocation(CarLocation);
	CarActors[CarIndex].StaticMesh->SetWorldRotation(CarRotation);
	CarActors[CarIndex].TraveledDistance = CarDistanceOnSpline;
}

void ARailSpline::SpawnMeshActor(const int CarIndex, UStaticMesh* ActorMeshType)
{
	const int CarDistanceOnSpline = GetCarSpawnDistance(CarIndex);

	const FVector CarLocation = Spline->GetLocationAtDistanceAlongSpline(CarDistanceOnSpline, ESplineCoordinateSpace::World);
	const FRotator CarRotation = Spline->GetRotationAtDistanceAlongSpline(CarDistanceOnSpline, ESplineCoordinateSpace::World);

	const FString Name = "Car" + CarIndex;
	UStaticMeshComponent* CarMesh = NewObject<UStaticMeshComponent>(this, FName(Name));
	CarMesh->SetupAttachment(RootComponent);
	CarMesh->RegisterComponent();
	CarMesh->SetStaticMesh(ActorMeshType);
	CarMesh->SetWorldLocation(CarLocation);
	CarMesh->SetWorldRotation(CarRotation);

	CarActors.Emplace(CarMesh, CarDistanceOnSpline);
}

float ARailSpline::GetCarSpawnDistance(const int CarIndex) const
{
	float CarDistance = (CarNumber + 1 - CarIndex) * CarSize;

	while(CarDistance < 0)
	{
		CarDistance = Spline->GetSplineLength() + CarDistance;
	}

	UE_LOG(LogTemp, Warning, TEXT("%d"), CarDistance)

	return CarDistance;
}

float ARailSpline::GetCarSplineDistance(const int CarIndex) const
{
	float CarDistance = CarActors[CarIndex].TraveledDistance + Speed;

	while(CarDistance > Spline->GetSplineLength())
	{
		CarDistance -= Spline->GetSplineLength();
	}

	return CarDistance;
}

void ARailSpline::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for(int i = 0; i < CarNumber + 1; i++)
	{
		MoveMeshActor(i);
	}
}
