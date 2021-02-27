#include "StaticTrap.h"


#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

AStaticTrap::AStaticTrap() : AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement->Bounciness = 0.5f;
	ProjectileMovement->Friction = 0.5;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->ProjectileGravityScale = 2;

	StaticMesh->SetWorldScale3D({ 0.25, 0.25, 0.25 });

	CollisionComponent->AddRelativeLocation({ 0, 0, 150 });
	CollisionComponent->InitCapsuleSize(250, 325);

	StaticMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Assets/Meshes/Bonus/SM_CymbalMonkey.SM_CymbalMonkey")).Object);

	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	ProjectileType = EProjectileType::MONKEY;
}

void AStaticTrap::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &AStaticTrap::OnStop);

	GetWorldTimerManager().ClearTimer(TimerHandle_TransformRefresh);
}

void AStaticTrap::OnStop(const FHitResult& ImpactResult)
{
	SetActorRotation(GetGroundRotation(ImpactResult));
}

void AStaticTrap::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
    const FRotator GroundRotation = GetGroundRotation(ImpactResult);

	if (UKismetMathLibrary::Abs(GroundRotation.Pitch) < 30 && UKismetMathLibrary::Abs(GroundRotation.Roll) < 30)
	{
		ProjectileMovement->StopMovementImmediately();
	}
}
