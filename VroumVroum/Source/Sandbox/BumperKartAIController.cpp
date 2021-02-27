// Fill out your copyright notice in the Description page of Project Settings.


#include "BumperKartAIController.h"
#include "Circuit.h"

void ABumperKartAIController::BeginPlay()
{
    Super::BeginPlay();

    if (!RunBehaviorTree(CarBehavior))
    {
        UE_LOG(LogTemp, Warning, TEXT("Car Behavior could not be run."));
    }
}
