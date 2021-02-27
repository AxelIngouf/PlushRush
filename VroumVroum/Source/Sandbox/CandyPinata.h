// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CandyPinata.generated.h"

UCLASS()
class SANDBOX_API ACandyPinata final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACandyPinata();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void	BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Pinata collision.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	class USphereComponent* CollisionComponent = nullptr;

	/**
	 * Pinata static mesh.
	 */
	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMesh = nullptr;

	/**
	 * Pinata candy emitter.
	 */
	UPROPERTY(EditAnywhere, Category = Candy)
	class UCandyEmitter* CandyEmitter = nullptr;

    /**
	 * Number of candies the pinata will drop when destroyed.
	 */
	UPROPERTY(EditAnywhere, Category = Candy)
	int	NumberOfCandies = 3;

private:
	void	Explode();

	void	RefreshTransform();

	FTimerHandle TimerHandle_TransformRefresh;
};
