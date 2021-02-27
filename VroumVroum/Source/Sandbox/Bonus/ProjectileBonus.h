// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BonusObject.h"
#include "ProjectileBonus.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UProjectileBonus final : public UBonusObject
{
	GENERATED_BODY()
public:
	UProjectileBonus();

	virtual void ActivateBonus(AActor* Owner) override;

	virtual void ActivateBonusBehind(AActor* Owner) override;

	void	SpawnProjectile(FVector Location, FRotator Rotation, AActor* Owner) const;

	/**
	 * Current projectile type thrown when activated.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileType = nullptr;
	
};
