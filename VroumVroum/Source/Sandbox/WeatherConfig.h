// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeatherConfig.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRainUpdate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SANDBOX_API UWeatherConfig : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeatherConfig();

	bool	IsRaining() const { return bRaining; }

	void	SetConstantRain(const bool bRain) { bConstantRain = bRain; }
	void	SetConstantNight(const bool bNight) { bConstantNight = bNight; }

	UPROPERTY(BlueprintAssignable)
	FOnRainUpdate OnRainUpdate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void	SimulateDay();

	void	InitLight();

	void	StartRain();

	void	StopRain();

	void	Thunder();

	void	ThunderLight(float Brightness) const;

	void	StopThunderLight();

	void	SetDirectionalLightBrightness(float Brightness) const;

    void	SetSunRotation(float SunPitchRotation) const;
	void	UpdateSunRotation() const;

	void	SetSkySphere();

	UPROPERTY()
	UParticleSystemComponent*	RainParticleSystemComponent = nullptr;
	UPROPERTY()
	USkyLightComponent*			WorldSkyLight = nullptr;
	class ADirectionalLight*	DirectionLight = nullptr;
	UPROPERTY()
	ADirectionalLight*			ThunderDirectionLight = nullptr;

	UPROPERTY()
	AActor* SkySphere = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool	bRaining = false;
	float	DefaultLightIntensity;
	int		ThunderFlashNumber;
	bool	bSunRising = false;
	float	RainSunBrightness=0.5;
	float	SunSpeed = 0.1;

	UPROPERTY(EditAnywhere)
	bool	bConstantRain = false;
	UPROPERTY(EditAnywhere)
	bool	bConstantNight = false;
	UPROPERTY(EditAnywhere)
	bool	bCanRain = true;
	UPROPERTY(EditAnywhere)
	bool	bSimulateDay = true;

    /**
	 * Timer between each rain episodes and rain duration.
	 */
	UPROPERTY(EditAnywhere)
	float	RainIntervals = 45.f;

	FTimerHandle	TimerHandle_Day;
	FTimerHandle	TimerHandle_Rain;
	FTimerHandle	TimerHandle_Thunder;
	FTimerHandle	TimerHandle_ThunderLight;

private:
	UPROPERTY()
	USoundWave*			Thunder1SoundWave;
	UPROPERTY()
	USoundWave*			Thunder2SoundWave;
	UParticleSystem*	RainParticleSystem;
};
