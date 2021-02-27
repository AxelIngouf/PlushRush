// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementContentFinalWidget.h"
#include "Components/CanvasPanelSlot.h"

void UAchievementContentFinalWidget::NativeConstruct()
{
	if (UCanvasPanelSlot* AchievementContentFinalWidgetSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		AchievementContentFinalWidgetSlot->SetSize(FVector2D(1920.f, 1080.f));
	}
}