// Fill out your copyright notice in the Description page of Project Settings.


#include "Bouncer.h"
#include "BounceComponent.h"

// Sets default values
ABouncer::ABouncer()
{
	PrimaryActorTick.bCanEverTick = false;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    RootComponent = StaticMesh;

    BounceComponent = CreateDefaultSubobject<UBounceComponent>(TEXT("Bounce Component"));
    BounceComponent->SetupAttachment(RootComponent);

}

