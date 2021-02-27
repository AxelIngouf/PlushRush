// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "StaticTrap.generated.h"

UCLASS()
class SANDBOX_API AStaticTrap final : public AProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStaticTrap();

protected:
	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;

	UFUNCTION()
	void			OnStop(const FHitResult& ImpactResult);

	virtual void	OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity) override;

};
