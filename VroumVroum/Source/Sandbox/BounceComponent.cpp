#include "BounceComponent.h"

#include "BumperKart.h"
#include "Kismet/KismetMathLibrary.h"

UBounceComponent::UBounceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BounceForce = 5;
}

void UBounceComponent::BeginPlay()
{
	Super::BeginPlay();

	this->OnComponentBeginOverlap.AddDynamic(this, &UBounceComponent::BeginOverlap);
}

void UBounceComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	if (Cast<ABumperKart>(OtherActor))
	{
		BounceActor(OtherActor, OtherComp);
	}
}

void UBounceComponent::BounceActor(AActor* OtherActor, UPrimitiveComponent* OtherComp) const
{
    const FVector Impulse = UKismetMathLibrary::GetDirectionUnitVector(this->GetComponentLocation(),
                                                                       OtherComp->GetComponentLocation()) * GetBouncePower(OtherActor->GetVelocity());
	OtherComp->AddImpulse(Impulse);
}

FVector UBounceComponent::GetBouncePower(const FVector OtherVelocity) const
{
	return FVector(BounceForce * GetOwner()->GetVelocity().X + BounceForce * OtherVelocity.X,
                   BounceForce * GetOwner()->GetVelocity().Y + BounceForce * OtherVelocity.Y, 0);
}

