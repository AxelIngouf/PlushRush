// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameAchievementWidget.h"
#include "EndGameWidget.h"
#include "UIDataHolderController.h"

#include "Components/CanvasPanelSlot.h"

#include "EndGameWidget.h"

void UEndGameAchievementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (UCanvasPanelSlot* EndGameAchievementWidgetSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		EndGameAchievementWidgetSlot->SetSize(FVector2D(1920.f, 1080.f));
	}

	PlayAnimation(FadeInAchievementThumbnail, 0, 1, EUMGSequencePlayMode::Forward, 1, false);
	
	FadeInAchievementThumbnailFinishedDelegate.BindUFunction(this, FName("FadeInAchievementThumbnailFinished"));
	
	
	BindToAnimationFinished(FadeInAchievementThumbnail, FadeInAchievementThumbnailFinishedDelegate);

}


void UEndGameAchievementWidget::FadeInAchievementThumbnailFinished()
{
	this->SetVisibility(ESlateVisibility::Hidden);

	UEndGameWidget* UIDataHolder = nullptr;

	if(auto Test = Cast<AUIDataHolderController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		UIDataHolder = Test->MyEndGameMainMenu;
	}

	if (UIDataHolder != nullptr)
	{
		 UIDataHolder->StartNewAchievementReading();
	}
	
}
