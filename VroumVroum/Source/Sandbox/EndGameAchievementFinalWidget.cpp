// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameAchievementFinalWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "UObject/ConstructorHelpers.h"
#include "UIDataHolderController.h"
#include "Kismet/GameplayStatics.h"


UEndGameAchievementFinalWidget::UEndGameAchievementFinalWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> CustomWidgetBP(TEXT("/Game/UI/Widgets/Widget_AchievementContentFinal"));
	if (CustomWidgetBP.Class != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found custom widget achievement class %s !"), *CustomWidgetBP.Class->GetName());

		AchievementContentFinalWidgetClass = CustomWidgetBP.Class;
	}
}



void UEndGameAchievementFinalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UCanvasPanelSlot* EndGameAchievementWidgetSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		EndGameAchievementWidgetSlot->SetSize(FVector2D(1920.f, 1080.f));
	}
	
	PlayAnimation(DisplayFinalAchievements, 0, 1, EUMGSequencePlayMode::Forward, 1, false);

	DisplayFinalAchievementsFinishedDelegate.BindUFunction(this, FName("DisplayFinalAchievementsFinished"));


	BindToAnimationFinished(DisplayFinalAchievements, DisplayFinalAchievementsFinishedDelegate);
}

void UEndGameAchievementFinalWidget::DisplayFinalAchievementsFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("End achievement read"));

	auto UIDataHolder = Cast<AUIDataHolderController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->MyEndGameMainMenu;

	if (UIDataHolder != nullptr)
	{
		UIDataHolder->EndAchievementDisplay();
	}
}

void UEndGameAchievementFinalWidget::CreateAchievementsContent()
{
	auto UIDataHolder = Cast<AUIDataHolderController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->MyEndGameMainMenu;

	for (FAchievement Achievement : UIDataHolder->GetAchievements())
	{
		AchievementContentFinalWidget = CreateWidget<UAchievementContentFinalWidget>(this, AchievementContentFinalWidgetClass);

		AchievementContentFinalWidget->AchievementThumbnail->SetBrushFromTexture(Achievement.Texture);
		AchievementHolder->AddChildToVerticalBox(AchievementContentFinalWidget);
	}
}
