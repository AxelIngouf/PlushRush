// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpeedBoost.generated.h"

class UBoxComponent;

UCLASS()
class SANDBOX_API ASpeedBoost : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpeedBoost();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxTrigger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BoostMult = 69000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnMyOverlapBegin(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
