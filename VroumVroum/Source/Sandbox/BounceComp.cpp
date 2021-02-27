// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceComp.h"
#include "BumperKart.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

ABounceComp::ABounceComp()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(FName("Trigger"));
	if (Trigger != nullptr)
	{
		Trigger->SetupAttachment(this->GetRootComponent());
		Trigger->SetGenerateOverlapEvents(true);
		Trigger->OnComponentHit.AddDynamic(this, &ABounceComp::OnCompHit);
	}
}

void ABounceComp::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABounceComp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABounceComp::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA<ABumperKart>())
	{
		FVector Normal = Hit.Normal * Trigger->GetRelativeLocation();

		FVector Reflect = FMath::GetReflectionVector(OtherActor->GetActorLocation(), Normal);
		Reflect *= BounceRatio * Cast<ABumperKart>(OtherActor)->GetVelocityVec();
		Cast<ABumperKart>(OtherActor)->BoxColl->AddImpulse({ Reflect.X, Reflect.Y, 0 });
	}
}