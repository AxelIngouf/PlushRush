// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


#include "EndGameAchievementWidget.generated.h"

/**
 * 
 */

UCLASS()
class SANDBOX_API UEndGameAchievementWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
	//bool Initialize() override;

	UFUNCTION()
        void FadeInAchievementThumbnailFinished();

    UPROPERTY(meta = (BindWidget))
        UImage* AchievementThumbnail;
    UPROPERTY(meta = (BindWidget))
        UTextBlock* AchievementDescription;
protected:
    UPROPERTY(meta = (BindWidgetAnim))
        UWidgetAnimation* FadeInAchievementThumbnail;


    FWidgetAnimationDynamicEvent FadeInAchievementThumbnailFinishedDelegate;
};
