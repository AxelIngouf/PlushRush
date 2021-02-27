// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderboardContentWidget.h"
#include "Components/CanvasPanelSlot.h"


void ULeaderboardContentWidget::NativeConstruct()
{
	if (UCanvasPanelSlot* LeaderboardContentWidgetSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		LeaderboardContentWidgetSlot->SetSize(FVector2D(1920.f, 1080.f));
	}
}
