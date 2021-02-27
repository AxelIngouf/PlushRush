#include "CandyEmitter.h"
#include "Candy.h"
#include "Kismet/KismetMathLibrary.h"

#define DISTANCE_FROM_EMITTER 200
#define PITCH_ANGLE 45

UCandyEmitter::UCandyEmitter()
{
	PrimaryComponentTick.bCanEverTick = false;

	CandyType = ACandy::StaticClass();
}

void UCandyEmitter::EmitCandies(const int NumberOfCandies) const
{
	if(NumberOfCandies > 0)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Owner = GetOwner();


		int Rotation = 180;
		const int AngleIncrement = 270 / NumberOfCandies;

		const FTransform ActorTransform = GetOwner()->GetActorTransform();
		const FVector ActorLocation = GetOwner()->GetActorLocation();

		for (int iCandy = 0; iCandy < NumberOfCandies; iCandy++)
		{
			FVector SpawnLocation = ActorLocation;
			SpawnLocation.X += DISTANCE_FROM_EMITTER * sin(Rotation);
			SpawnLocation.Y += DISTANCE_FROM_EMITTER * cos(Rotation);

			FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::InverseTransformLocation(ActorTransform, SpawnLocation));
			SpawnRotation.Pitch += PITCH_ANGLE;

			if(UWorld* World = GetWorld())
			{
		        World->SpawnActor<ACandy>(CandyType, SpawnLocation, SpawnRotation, SpawnParameters);
			}


			Rotation += AngleIncrement;
		}
	}
}

