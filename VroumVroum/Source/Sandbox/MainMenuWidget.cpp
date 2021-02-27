
// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayButton != nullptr)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::StartGame);
	}

}

void UMainMenuWidget::StartGame()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	UGameplayStatics::OpenLevel(GetWorld(), "PlaceHolder");

}
