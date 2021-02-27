// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

#include "AchievementContentFinalWidget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UAchievementContentFinalWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		UImage* AchievementThumbnail;
	
};
