// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CandyEmitter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UCandyEmitter : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCandyEmitter();

	void	EmitCandies(int NumberOfCandies) const;

protected:
	/**
	 * Current candy type thrown when activated.
	 */
	UPROPERTY(EditAnywhere, Category = Bonus)
	TSubclassOf<class ACandy> CandyType = nullptr;
};
