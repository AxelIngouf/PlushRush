// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BonusObject.generated.h"


UENUM(BlueprintType)
enum class EBonusType : uint8
{
	PROJECTILE_ONE			UMETA(DisplayName = "ProjectileOne"),
	PROJECTILE_THREE		UMETA(DisplayName = "ProjectileThree"),
	STATIC_TRAP				UMETA(DisplayName = "StaticTrap"),
	REMOTE_CONTROLLED_CAR	UMETA(DisplayName = "RemoteControlledCar"),

	EMPTY					UMETA(DisplayName = "Empty"),
	PROJECTILE_TWO			UMETA(DisplayName = "ProjectileTwo")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SANDBOX_API UBonusObject : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UBonusObject() = default;

    /**
	 * Activates the BonusObject and indicates if it was completely used.
	 * If the returned value is STILL_USABLE, then the bonus can still be used by its owner and should not be destroyed.
	 *
	 * @return enum result stating if the bonus was used or if it's still usable
	 */
	virtual void ActivateBonus(AActor* Owner) { }

	virtual void ActivateBonusBehind(AActor* Owner) { return ActivateBonus(Owner); }

    /**
	 * Retrieves current BonusObject type.
	 * 
	 * @return current BonusObject type.
	 */
	EBonusType		GetBonusType() const { return BonusType; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EBonusType BonusType = EBonusType::EMPTY;
};