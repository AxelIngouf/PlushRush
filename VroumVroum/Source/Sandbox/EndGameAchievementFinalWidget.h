// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "AchievementContentFinalWidget.h"
#include "EndGameAchievementFinalWidget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UEndGameAchievementFinalWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UEndGameAchievementFinalWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	UFUNCTION()
		void DisplayFinalAchievementsFinished();
	UFUNCTION()
		void CreateAchievementsContent();
protected:
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* DisplayFinalAchievements;
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* AchievementHolder;

	UPROPERTY()
		UAchievementContentFinalWidget* AchievementContentFinalWidget;

	TSubclassOf<UUserWidget> AchievementContentFinalWidgetClass;

	FWidgetAnimationDynamicEvent DisplayFinalAchievementsFinishedDelegate;
};
