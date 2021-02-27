// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeedBoost.h"
#include "BumperKart.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpeedBoost::ASpeedBoost()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(FName("BoxTrigger"));
	if (BoxTrigger != nullptr)
	{
		BoxTrigger->SetupAttachment(this->GetRootComponent());
		BoxTrigger->SetGenerateOverlapEvents(true);
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASpeedBoost::OnMyOverlapBegin);
	}
}

// Called when the game starts or when spawned
void ASpeedBoost::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpeedBoost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpeedBoost::OnMyOverlapBegin(UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ABumperKart>())
	{
		FVector Impulse = Cast<ABumperKart>(OtherActor)->GetActorForwardVector();
		FVector Extent = Cast<ABumperKart>(OtherActor)->BoxColl->GetScaledBoxExtent();
		FVector Loc = Cast<ABumperKart>(OtherActor)->GetActorLocation();
		Loc.X -= Extent.X;
		Impulse *= BoostMult;

		Cast<ABumperKart>(OtherActor)->BoxColl->AddForceAtLocation(Impulse, { Loc.X, Loc.Y, 0 });
	}
}