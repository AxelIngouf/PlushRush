// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bouncer.generated.h"

UCLASS()
class SANDBOX_API ABouncer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABouncer();

protected:

	/**
	 * Bouncer static mesh.
	 */
	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMesh = nullptr;

	/**
	* Bouncer bounce component.
	*/
	UPROPERTY(EditAnywhere, Category = Bounce)
	class UBounceComponent* BounceComponent = nullptr;

};
