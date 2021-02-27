#pragma once

#include "CoreMinimal.h"
#include "Bonus/Projectile.h"
#include "Candy.generated.h"

UCLASS()
class SANDBOX_API ACandy : public AProjectile
{
	GENERATED_BODY()
	
public:	
	ACandy();

protected:
    virtual void	BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void	OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity) override;

	virtual void RefreshTransform() override;

private:
	bool	bAscending = true;
	bool	bOnGround = false;
	FVector TargetLocation;
};
