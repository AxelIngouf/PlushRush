#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "BounceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UBounceComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:	
	UBounceComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void	BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void	BounceActor(AActor* OtherActor, UPrimitiveComponent* OtherComp) const;

	FVector	GetBouncePower(FVector OtherVelocity) const;
	
	UPROPERTY(EditAnywhere, Category = Collision)
	class UCapsuleComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = Bounce)
	int	MinBouncePower;

	UPROPERTY(EditAnywhere, Category = Bounce)
	int BounceForce;

};
