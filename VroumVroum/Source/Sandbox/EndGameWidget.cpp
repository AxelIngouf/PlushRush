// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "PlushRushInstance.h"

#include "BumperKart.h"

UEndGameWidget::UEndGameWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> CustomWidgetBP(TEXT("/Game/UI/Widgets/Widget_EndGameAchievement"));
	if (CustomWidgetBP.Class != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found custom widget achievement class %s !"), *CustomWidgetBP.Class->GetName());

		CustomWidgetClass = CustomWidgetBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CustomWidgetBP2(TEXT("/Game/UI/Widgets/Widget_EndGameAchievementFinal"));
	if (CustomWidgetBP2.Class != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found custom widget achievement class %s !"), *CustomWidgetBP2.Class->GetName());

		EndGameAchievementFinalWidgetClass = CustomWidgetBP2.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CustomWidgetBP3(TEXT("/Game/UI/Widgets/Widget_LeaderboardContent"));
	if (CustomWidgetBP3.Class != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found custom widget achievement class %s !"), *CustomWidgetBP3.Class->GetName());

		LeaderboardContentWidgetClass = CustomWidgetBP3.Class;
	}

	
}

void UEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());
	if (RootWidget != nullptr)
	{
		//TODO: Redo the find in array to find the name of the canvas or something like that

	}
	if (GoToStatsButton != nullptr)
	{
		GoToStatsButton->OnClicked.AddDynamic(this, &UEndGameWidget::SwapPanel);
		GoToRankingButton->OnClicked.AddDynamic(this, &UEndGameWidget::SwapPanel);
		GoToMainMenuButton->OnClicked.AddDynamic(this, &UEndGameWidget::GoToMainMenu);
	}

	if (StartAchievementDisplayButton != nullptr)
	{
		StartAchievementDisplayButton->OnClicked.AddDynamic(this, &UEndGameWidget::DisplayAchievements);
	}

	int NumberOfPlayerInRace = Cast<UPlushRushInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->NumberOfPlayersInRace;

	for (int iPlayer = 0; iPlayer < NumberOfPlayerInRace; iPlayer++)
	{
		LeaderboardContentWidget = CreateWidget<ULeaderboardContentWidget>(this, LeaderboardContentWidgetClass);

		if (iPlayer == 0)
		{
			LeaderboardContentWidget->CharacterThumbnail->SetBrushFromTexture(PandaThumbnail);
		}
		else if (iPlayer == 1)
		{
			LeaderboardContentWidget->CharacterThumbnail->SetBrushFromTexture(CatThumbnail);

		}
		else if (iPlayer == 2)
		{
			LeaderboardContentWidget->CharacterThumbnail->SetBrushFromTexture(DinoThumbnail);

		}
		FString placement;
		placement.AppendInt(iPlayer + 1);
		LeaderboardContentWidget->Placement->SetText(FText::FromString(placement));
		LeaderboardBox->AddChildToVerticalBox(LeaderboardContentWidget);
	}
	
	

}



void UEndGameWidget::SwapPanel()
{
	if (m_ShouldSlideLeft == true)
	{
		PlayAnimation(RankingSlideLeftAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, false);
		PlayAnimation(StatsSlideLeftAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, false);
		GoToRankingButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		PlayAnimation(RankingSlideLeftAnimation, 0, 1, EUMGSequencePlayMode::Reverse, 1, false);
		PlayAnimation(StatsSlideLeftAnimation, 0, 1, EUMGSequencePlayMode::Reverse, 1, false);
	}
	m_ShouldSlideLeft = !m_ShouldSlideLeft;
}

void UEndGameWidget::StartNewAchievementReading()
{
	UE_LOG(LogTemp, Warning, TEXT("Should start new achievement"));
	CreateAndDisplayAchievement();
}

void UEndGameWidget::CreateAndDisplayAchievement()
{

	if (CurrentAchievementIndexToRead >= Achievements.Num())
	{
		auto StatsPanelCast = Cast<UPanelWidget>(StatsPanel->GetChildAt(0));

		EndGameAchievementFinalWidget = CreateWidget<UEndGameAchievementFinalWidget>(this, EndGameAchievementFinalWidgetClass);
		//EndGameAchievementFinalWidget->AchievementThumbnail->SetBrushFromTexture(Achievements[CurrentAchievementIndexToRead].Texture);
		StatsPanelCast->AddChild(EndGameAchievementFinalWidget);

		EndGameAchievementFinalWidget->CreateAchievementsContent();
		
		return;
	}


	auto StatsPanelCast = Cast<UPanelWidget>(StatsPanel->GetChildAt(0));

	CustomWidget = CreateWidget<UEndGameAchievementWidget>(this, CustomWidgetClass);
	CustomWidget->AchievementThumbnail->SetBrushFromTexture(Achievements[CurrentAchievementIndexToRead].Texture);

	CustomWidget->AchievementDescription->SetText(FText::FromString(Achievements[CurrentAchievementIndexToRead].Description));
	StatsPanelCast->AddChild(CustomWidget);

	CurrentAchievementIndexToRead++;

}

void UEndGameWidget::DisplayAchievements()
{
	StartAchievementDisplayButton->SetVisibility(ESlateVisibility::Hidden);

	CreateAndDisplayAchievement();
}

void UEndGameWidget::GoToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
}

void UEndGameWidget::EndAchievementDisplay()
{
	GoToMainMenuButton->SetVisibility(ESlateVisibility::Visible);
}
