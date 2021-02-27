#include "CandyPinata.h"

#include "BumperKart.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "CandyEmitter.h"

ACandyPinata::ACandyPinata()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	StaticMesh->SetupAttachment(RootComponent);

	CandyEmitter = CreateDefaultSubobject<UCandyEmitter>(TEXT("CandyEmitter"));
}

void ACandyPinata::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ACandyPinata::BeginOverlap);

	GetWorldTimerManager().SetTimer(TimerHandle_TransformRefresh, this, &ACandyPinata::RefreshTransform,
		0.03f, true);
}

void ACandyPinata::BeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ABumperKart* BumperKart = Cast<ABumperKart>(OtherActor);

	if (BumperKart)
	{
		BumperKart->AddCandy();
		BumperKart->AddCandy();
	}

	Explode();
}

void ACandyPinata::Explode()
{
	CandyEmitter->EmitCandies(NumberOfCandies);
	Destroy();
}

void ACandyPinata::RefreshTransform()
{
	AddActorLocalOffset(FVector(0, 0, sin(GetGameTimeSinceCreation()) / 2));
}

