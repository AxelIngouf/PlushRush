// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "RemoteControlledCar.generated.h"

class ABumperKart;
class ACircuit;
struct FCircuitPlayer;
class ACheckpoint;

UCLASS()
class SANDBOX_API ARemoteControlledCar final : public ACharacter
{
	GENERATED_BODY()

public:
	ARemoteControlledCar();

	void			Explode();

	FCircuitPlayer* GetTarget() const;

	ACheckpoint*	GetNextCheckpoint() const { return NextCheckpoint; }
	void			SetNextCheckpoint(ACheckpoint* Checkpoint);

	void	SetEmitter(ABumperKart* Emitter);

protected:
	virtual void	BeginPlay() override;

	UFUNCTION()
	void	BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * RC Car static mesh.
	 */
	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = Explosion)
	int	ExplosionRadius = 150;

	ABumperKart* CarEmitter;

private:
	void	RefreshTransform();

	FTimerHandle	TimerHandle_TransformRefresh;

	ACheckpoint*	NextCheckpoint = nullptr;

	UPROPERTY(EditAnywhere)
	int		KnockOverPlayerTime = 4.f;

	class ACircuit*		Circuit = nullptr;
};
