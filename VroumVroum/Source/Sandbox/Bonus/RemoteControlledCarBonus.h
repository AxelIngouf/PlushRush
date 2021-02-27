// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BonusObject.h"
#include "RemoteControlledCarBonus.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API URemoteControlledCarBonus : public UBonusObject
{
	GENERATED_BODY()

public:
	URemoteControlledCarBonus();

	virtual void ActivateBonus(AActor* Owner) override;

};
