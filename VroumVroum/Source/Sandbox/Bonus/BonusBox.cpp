// Fill out your copyright notice in the Description page of Project Settings.

#include "BonusBox.h"
#include "Components/BoxComponent.h"
#include "Sandbox/BumperKart.h"

// Sets default values
ABonusBox::ABonusBox()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	StaticMesh->SetupAttachment(RootComponent);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABonusBox::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABonusBox::BeginOverlap);


	GetWorldTimerManager().SetTimer(TimerHandle_TransformRefresh, this, &ABonusBox::RefreshTransform, 0.03f, true);
}

void ABonusBox::BeginOverlap(UPrimitiveComponent* , AActor* OtherActor,
                             UPrimitiveComponent* , int32 , bool ,
                             const FHitResult& )
{
	ABumperKart* BumperKart = Cast<ABumperKart>(OtherActor);

	if(!bUsed && BumperKart)
	{
		BumperKart->PickUpItem();
		StaticMesh->SetVisibility(false);
		bUsed = true;

		GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ABonusBox::Respawn, TimeBeforeRespawn);
	}

}

void ABonusBox::Respawn()
{
	StaticMesh->SetVisibility(true);
	bUsed = false;
}

void ABonusBox::RefreshTransform()
{
	const float SinTime = sin(GetGameTimeSinceCreation());

	AddActorLocalRotation(FRotator(cos(GetGameTimeSinceCreation()), SinTime, SinTime));
}
