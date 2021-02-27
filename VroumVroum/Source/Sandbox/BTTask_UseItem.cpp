// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UseItem.h"

#include "AIController.h"
#include "BumperKart.h"
#include "Bonus/BonusObject.h"
#include "Circuit.h"

EBTNodeResult::Type UBTTask_UseItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if(!Kart)
        Kart = Cast<ABumperKart>(OwnerComp.GetAIOwner()->GetPawn());

    switch(static_cast<EBonusType>(Kart->GetCurrentBonus()))
    {
    case EBonusType::PROJECTILE_ONE:
        UseProjectile();
        break;
    case EBonusType::PROJECTILE_TWO:
        UseProjectile();
        break;
    case EBonusType::PROJECTILE_THREE:
        UseProjectile();
        break;
    case EBonusType::STATIC_TRAP:
        UseStaticTrap();
        break;
    case EBonusType::REMOTE_CONTROLLED_CAR:
        UseRemoteControlledCar();
        break;
    default:
        break;
    }

    return EBTNodeResult::Succeeded;
}

void UBTTask_UseItem::UseProjectile() const
{
    Kart->AttemptToUseItem();
}

void UBTTask_UseItem::UseStaticTrap() const
{
    Kart->AttemptToUseItem();
}

void UBTTask_UseItem::UseRemoteControlledCar() const
{
    if(Kart->GetCircuit()->GetFirstPlayerInRace()->Actor != Kart)
    {
        Kart->AttemptToUseItem();
    }
}
