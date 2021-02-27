// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void StartGame();
protected:
	UPROPERTY(meta = (BindWidget))
		UButton* PlayButton;
};
