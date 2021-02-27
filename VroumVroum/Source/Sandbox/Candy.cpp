#include "Candy.h"


#include "BumperKart.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

#define ROTATION_SPEED 1
#define DISTANCE_ABOVE_GROUND 70

ACandy::ACandy()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent->AddRelativeLocation({ 0, 0, -50 });
	CollisionComponent->InitCapsuleSize(50, 50);

	ProjectileMovement->InitialSpeed = 500.0f;
	ProjectileMovement->Bounciness = 0.5f;
	ProjectileMovement->Friction = 1;

	ProjectileType = EProjectileType::CANDY;
}

void ACandy::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABumperKart* BumperKart = Cast<ABumperKart>(OtherActor);

	if (BumperKart)
	{
		BumperKart->AddCandy();
		Destroy();
	}
}

void ACandy::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	const FRotator GroundRotation = GetGroundRotation(ImpactResult);

	if (UKismetMathLibrary::Abs(GroundRotation.Pitch) < 30 && UKismetMathLibrary::Abs(GroundRotation.Roll) < 30)
	{
		ProjectileMovement->StopMovementImmediately();
		bOnGround = true;

		TargetLocation = GetActorLocation();
		TargetLocation.Z += DISTANCE_ABOVE_GROUND;
	}
}

void ACandy::RefreshTransform()
{
	if (bOnGround)
	{
		const float SinTime = sin(GetGameTimeSinceCreation());
		if (bAscending)
		{
			SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, REFRESH_TRANSFORM_INTERVAL, 1));
			if ((GetActorLocation() - TargetLocation).IsNearlyZero(0.2))
				bAscending = false;
		}
		else
		{
			AddActorWorldOffset(FVector(0, 0, SinTime / 2));
		}
		AddActorWorldRotation(FRotator(0, ROTATION_SPEED, SinTime / 2));
	}
}

