// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

#define REFRESH_TRANSFORM_INTERVAL 0.03f

class ABumperKart;
UCLASS()
class SANDBOX_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:

	enum class EProjectileType
	{
	    BULLET,
		MONKEY,
		CANDY
	};

	// Sets default values for this actor's properties
	AProjectile();

	virtual void BeginPlay() override;

	void	SetEmitter(ABumperKart* Emitter);

	/**
	 * Projectile collision.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	class UCapsuleComponent* CollisionComponent;

	/**
	 * Projectile static mesh.
	 */
	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMesh = nullptr;


    /**
	 * Projectile movement component.
	 */
	UPROPERTY(EditAnywhere, Category = Mesh)
	class UProjectileMovementComponent* ProjectileMovement;


protected:
	UFUNCTION()
    virtual void	BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void	OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	FRotator		GetGroundRotation(const FHitResult& ImpactResult) const;

	virtual void	RefreshTransform();

	int		BounceNumber = 0;

	UPROPERTY(EditAnywhere, Category = Bounce)
	int		MaxBounce;

	UPROPERTY(EditAnywhere)
	int		KnockOverPlayerTime = 2.f;

	ABumperKart* ProjectileEmitter;

	EProjectileType ProjectileType;

	FTimerHandle TimerHandle_TransformRefresh;
};
