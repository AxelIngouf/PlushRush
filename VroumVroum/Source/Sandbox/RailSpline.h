// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RailSpline.generated.h"

class AStaticMeshActor;
class USplineMeshComponent;
class USplineComponent;

USTRUCT()
struct FRailCar
{
	GENERATED_BODY()

	FRailCar() = default;
    explicit FRailCar(UStaticMeshComponent* Mesh, const int SpawnDistance) : StaticMesh(Mesh), TraveledDistance(SpawnDistance) {}

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	float	TraveledDistance = 0.f;
};

UCLASS()
class SANDBOX_API ARailSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	ARailSpline();

	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int		CarNumber = 3;

	UPROPERTY(EditAnywhere)
	float	CarSize = 500.f;

	UPROPERTY(EditAnywhere)
	float	Speed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent*	TopScene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent*	Spline;

	UPROPERTY(EditAnywhere)
	UStaticMesh*	CarStaticMeshType;

	UPROPERTY(EditAnywhere)
	UStaticMesh*	LocomotiveStaticMeshType;

	UPROPERTY(VisibleAnywhere)
	TArray<FRailCar>	CarActors;

private:
	void	MoveMeshActor(int CarIndex);

	void	SpawnMeshActor(int CarIndex, UStaticMesh* ActorMeshType);

	float	GetCarSpawnDistance(int CarIndex) const;

	float	GetCarSplineDistance(int CarIndex) const;

	float	TraveledDistance = 0.f;
};
