// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherConfig.h"

#include "BumperKart.h"

#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"

#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Core/Public/Misc/OutputDeviceNull.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values for this component's properties
UWeatherConfig::UWeatherConfig()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USoundWave> T1S(TEXT("SoundWave'/Game/Audio/S_Thunder1.S_Thunder1'"));
	Thunder1SoundWave = T1S.Object;
	static ConstructorHelpers::FObjectFinder<USoundWave> T2S(TEXT("SoundWave'/Game/Audio/S_Thunder2.S_Thunder2'"));
	Thunder2SoundWave = T2S.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/FX/Flares/P_RainStorm.P_RainStorm'"));
	RainParticleSystem = PS.Object;
}


// Called when the game starts
void UWeatherConfig::BeginPlay()
{
	Super::BeginPlay();

	InitLight();

	SetSkySphere();

	if (bConstantNight)
	{
		DirectionLight->SetActorRotation({90, 0, 0});
		UpdateSunRotation();
		SetDirectionalLightBrightness(RainSunBrightness);
	}
    else if (bSimulateDay)
	{
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Day, this, &UWeatherConfig::SimulateDay, 0.03, true);
	}

	if (!bCanRain)
		return;

	Thunder();

	if(bConstantRain)
	{
		StartRain();
	}
	else
	{
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Rain, this, &UWeatherConfig::StartRain, RainIntervals);
	}
}

void UWeatherConfig::SimulateDay()
{
	if(bConstantNight)
	{
		SetDirectionalLightBrightness(RainSunBrightness);
	}
	else if(SkySphere)
	{
		const float CurrentSunRotation = DirectionLight->GetActorRotation().Pitch;

	    SetSunRotation(CurrentSunRotation + SunSpeed);

		if (abs(CurrentSunRotation + SunSpeed) > 90 || abs(CurrentSunRotation - SunSpeed) > 90)
			bSunRising = !bSunRising;

		if(!bRaining)
	        SetDirectionalLightBrightness(DefaultLightIntensity);
	}
}

void UWeatherConfig::InitLight()
{
	AActor* DirectionalLightActor = UGameplayStatics::GetActorOfClass(GetOwner(), ADirectionalLight::StaticClass());

	if (DirectionalLightActor && DirectionalLightActor->IsA<ADirectionalLight>())
	{
		DirectionLight = Cast<ADirectionalLight>(DirectionalLightActor);
		DefaultLightIntensity = DirectionLight->GetLightComponent()->Intensity;
	}

	AActor* SkyLightActor = UGameplayStatics::GetActorOfClass(GetOwner(), ASkyLight::StaticClass());

	if (SkyLightActor && SkyLightActor->IsA<ASkyLight>())
	{
		WorldSkyLight = Cast<ASkyLight>(SkyLightActor)->GetLightComponent();
	}


	ThunderDirectionLight = GetOwner()->GetWorld()->SpawnActorDeferred<ADirectionalLight>(ADirectionalLight::StaticClass(), FTransform(FRotator(90, 0, 0), FVector{ 0, 0, 1000 }));
	ThunderDirectionLight->SetMobility(EComponentMobility::Movable);
	ThunderDirectionLight->SetActorRotation({ -60, 0, 0 });
	ThunderDirectionLight->GetLightComponent()->SetIntensity(0);
	ThunderDirectionLight->FinishSpawning(FTransform(FRotator(90, 0, 0), FVector{ 0, 0, 1000 }));
}

void UWeatherConfig::StartRain()
{
	ABumperKart* BumperKart = Cast<ABumperKart>(UGameplayStatics::GetPlayerPawn(GetOwner(), 0));

	if(BumperKart)
	{
		RainParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(RainParticleSystem, BumperKart->BoxColl, NAME_None, FVector(ForceInit), FRotator::ZeroRotator, FVector(2));
	}

	if(!bConstantRain)
	{
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Rain, this, &UWeatherConfig::StopRain, RainIntervals);
	}

	bRaining = true;
	SetDirectionalLightBrightness(RainSunBrightness);

	OnRainUpdate.Broadcast();
}

void UWeatherConfig::StopRain()
{
	if(RainParticleSystemComponent != nullptr)
	{
		RainParticleSystemComponent->DestroyComponent();
		RainParticleSystemComponent = nullptr;
	}

	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Rain, this, &UWeatherConfig::StartRain, RainIntervals);

	SetDirectionalLightBrightness(DefaultLightIntensity);

	bRaining = false;

	OnRainUpdate.Broadcast();
}

void UWeatherConfig::Thunder()
{
	if(bRaining)
	{
		USoundBase* ThunderSound;
	    if(UKismetMathLibrary::RandomBool())
	    {
			ThunderSound = Thunder1SoundWave;
	    }
		else
		{
			ThunderSound = Thunder2SoundWave;
		}
		UGameplayStatics::PlaySound2D(this, ThunderSound);

		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_ThunderLight, this, &UWeatherConfig::StopThunderLight, 0.4);

		ThunderLight(3.f);
	}

	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Thunder, this, &UWeatherConfig::Thunder, UKismetMathLibrary::RandomIntegerInRange(8, 25));
}

void UWeatherConfig::ThunderLight(const float Brightness) const
{
	ThunderDirectionLight->GetLightComponent()->SetIntensity(Brightness);
}

void UWeatherConfig::StopThunderLight()
{
	if(ThunderFlashNumber == 3)
	{
		ThunderFlashNumber = 0;
		ThunderLight(0.f);
	}
	else
	{
		ThunderLight(UKismetMathLibrary::RandomFloatInRange(1.f, 3 - ThunderFlashNumber % 2));

		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_ThunderLight, this, &UWeatherConfig::StopThunderLight, 0.1);

		ThunderFlashNumber++;
	}
}

void UWeatherConfig::SetDirectionalLightBrightness(const float Brightness) const
{
	if(DirectionLight)
		DirectionLight->GetLightComponent()->SetIntensity(Brightness);
}

void UWeatherConfig::SetSunRotation(const float SunPitchRotation) const
{
	if(DirectionLight)
	{
		FRotator SunRotation = FRotator::ZeroRotator;
		SunRotation.Pitch = SunPitchRotation - DirectionLight->GetActorRotation().Pitch;
		DirectionLight->AddActorLocalRotation(SunRotation);

		UpdateSunRotation();
	}
}

void UWeatherConfig::UpdateSunRotation() const
{
	if (SkySphere)
	{
		FOutputDeviceNull Args;
		SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), Args, nullptr, true);
	}
}

void UWeatherConfig::SetSkySphere()
{
	for (TActorIterator<AActor> ActorItr(GetOwner()->GetWorld()); ActorItr; ++ActorItr)
	{
		if ((*ActorItr)->GetName().Contains("SkySphere") || (*ActorItr)->GetName().Contains("Sky_Sphere"))
		{
			SkySphere = *ActorItr;
			return;
		}
	}
}

