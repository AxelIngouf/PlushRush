// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "EndGameAchievementWidget.h"
#include "EndGameAchievementFinalWidget.h"
#include "Blueprint/WidgetTree.h"
#include "LeaderboardContentWidget.h"


#include "EndGameWidget.generated.h"


USTRUCT(BlueprintType)
struct FAchievement
{
    GENERATED_USTRUCT_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
        UTexture2D* Texture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
        FString Description;
};
/**
 *
 */
UCLASS()
class SANDBOX_API UEndGameWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UEndGameWidget(const FObjectInitializer& ObjectInitializer);
    virtual void NativeConstruct() override;

    UFUNCTION()
        void SwapPanel();
    UFUNCTION()
        void StartNewAchievementReading();
    UFUNCTION()
        void CreateAndDisplayAchievement();
    UFUNCTION()
        void DisplayAchievements();
    UFUNCTION()
        void GoToMainMenu();
    UFUNCTION()
        void EndAchievementDisplay();

    UFUNCTION()
        TArray<FAchievement>& GetAchievements() { return Achievements; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
		UTexture2D*  PandaThumbnail;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
        UTexture2D* CatThumbnail;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
        UTexture2D* DinoThumbnail;
   
protected:



    UPROPERTY(meta = (BindWidgetAnim))
        UWidgetAnimation* StatsSlideLeftAnimation;
    UPROPERTY(meta = (BindWidgetAnim))
        UWidgetAnimation* RankingSlideLeftAnimation;
    UPROPERTY(meta = (BindWidget))
        UButton* GoToStatsButton;
    UPROPERTY(meta = (BindWidget))
        UButton* GoToRankingButton;
    UPROPERTY(meta = (BindWidget))
        UButton* GoToMainMenuButton;
    UPROPERTY(meta = (BindWidget))
        UVerticalBox* LeaderboardBox;

    UPROPERTY(meta = (BindWidget))
        UButton* StartAchievementDisplayButton;
    UPROPERTY(meta = (BindWidget))
        UBorder* StatsPanel;

    bool m_ShouldSlideLeft = true;


    //TODO: Rename the custom
    UPROPERTY()
        UEndGameAchievementWidget* CustomWidget;

    TSubclassOf<UUserWidget> CustomWidgetClass;

    UPROPERTY()
        UEndGameAchievementFinalWidget* EndGameAchievementFinalWidget;

    TSubclassOf<UUserWidget> EndGameAchievementFinalWidgetClass;

    UPROPERTY()
        ULeaderboardContentWidget* LeaderboardContentWidget;

    TSubclassOf<UUserWidget> LeaderboardContentWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AchievementThumbnails)
        TArray<FAchievement> Achievements;

    int CurrentAchievementIndexToRead = 0;
};
