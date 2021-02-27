#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BonusBox.generated.h"

UCLASS()
class SANDBOX_API ABonusBox final : public AActor
{
	GENERATED_BODY()
	
public:	
	ABonusBox();

	virtual void BeginPlay() override;

    UFUNCTION()
    void	BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

	void	Respawn();

	UPROPERTY(EditAnywhere)
	float	TimeBeforeRespawn = 15.f;

	/**
	 * Bonus box collision.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	class UBoxComponent* CollisionComponent = nullptr;

	/**
	 * Bonus box static mesh.
	 */
	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMesh = nullptr;

private:
	void	RefreshTransform();

	bool	bUsed = false;

	FTimerHandle TimerHandle_TransformRefresh;
	FTimerHandle TimerHandle_Respawn;
};
