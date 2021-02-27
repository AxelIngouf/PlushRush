// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BounceComp.generated.h"

class USphereComponent;

UCLASS()
class SANDBOX_API ABounceComp : public AActor
{
	GENERATED_BODY()

public:
	ABounceComp();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* Trigger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BounceRatio = 42.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
