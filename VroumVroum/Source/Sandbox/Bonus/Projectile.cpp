// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sandbox/BumperKart.h"
#include "Sandbox/Circuit.h"
#include "UObject/ConstructorHelpers.h"

#define DISTANCE_FROM_GROUND 125
#define GRAVITY 1

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Assets/Meshes/Bonus/SM_Bullet.SM_Bullet")).Object);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootComponent = StaticMesh;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovement->SetUpdatedComponent(StaticMesh);
	ProjectileMovement->MaxSpeed = 4500.0f;
	ProjectileMovement->InitialSpeed = 4500.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 1.f;
	ProjectileMovement->Friction = 0;

	SetReplicates(true);

	MaxBounce = 5;

	ProjectileType = EProjectileType::BULLET;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AProjectile::OnBounce);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::BeginOverlap);

	GetWorldTimerManager().SetTimer(TimerHandle_TransformRefresh, this, &AProjectile::RefreshTransform, REFRESH_TRANSFORM_INTERVAL, true);
}

void AProjectile::SetEmitter(ABumperKart* Emitter)
{
	ProjectileEmitter = Emitter;
}

void AProjectile::RefreshTransform()
{
	FVector Target(GetActorLocation());
	Target.Z -= DISTANCE_FROM_GROUND;
	FHitResult HitResult;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, this->GetActorLocation(), Target, ECC_Visibility))
	{
		ProjectileMovement->ProjectileGravityScale = GRAVITY;
	}
	else
	{
		ProjectileMovement->ProjectileGravityScale = 0;
		if (HitResult.Location.Z + DISTANCE_FROM_GROUND > GetActorLocation().Z)
		{
			FVector NewLocation = GetActorLocation();
			NewLocation.Z = HitResult.Location.Z + DISTANCE_FROM_GROUND;
			SetActorLocation(NewLocation);
		}
	}
}

void AProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABumperKart* BumperKart = Cast<ABumperKart>(OtherActor);

	if(BumperKart)
	{
		BumperKart->KnockOver(KnockOverPlayerTime);

		if (ProjectileEmitter != nullptr && BumperKart->GetPlayerID() != ProjectileEmitter->GetPlayerID())
		{
			if(ProjectileType == EProjectileType::BULLET)
			    BumperKart->GetCircuit()->GetPlayer(ProjectileEmitter->GetPlayerID())->PlayerStats.PlayerTrappedWithBullet++;
			else if(ProjectileType == EProjectileType::MONKEY)
				BumperKart->GetCircuit()->GetPlayer(ProjectileEmitter->GetPlayerID())->PlayerStats.PlayerTrappedWithMonkey++;
		}

		Destroy();
	}

	if(Cast<AProjectile>(OtherActor) && OtherActor != this)
	{
		Destroy();
		OtherActor->Destroy();
	}
}

void AProjectile::OnBounce(const FHitResult&, const FVector&)
{

	BounceNumber++;
	if(BounceNumber >= MaxBounce)
	{
		Destroy();
	}
}

FRotator AProjectile::GetGroundRotation(const FHitResult& ImpactResult) const
{
	return 	UKismetMathLibrary::RInterpTo(
		GetActorRotation(),
		UKismetMathLibrary::MakeRotator(
			UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), ImpactResult.ImpactNormal).Roll,
			UKismetMathLibrary::MakeRotFromYZ(GetActorRightVector(), ImpactResult.ImpactNormal).Pitch,
			GetActorRotation().Yaw),
		1,
		10
	);
}
