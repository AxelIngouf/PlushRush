// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BonusObject.h"
#include "StaticTrapBonus.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UStaticTrapBonus final : public UBonusObject
{
	GENERATED_BODY()

public:
	UStaticTrapBonus();

	virtual void ActivateBonus(AActor* Owner) override;

	virtual void ActivateBonusBehind(AActor* Owner) override;

	/**
	 * Current projectile type thrown when activated.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AStaticTrap> ProjectileType = nullptr;
	
};
