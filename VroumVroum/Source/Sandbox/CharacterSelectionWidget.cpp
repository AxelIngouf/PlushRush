// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelectionWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UCharacterSelectionWidget::UCharacterSelectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> CustomWidgetBP(TEXT("/Game/UI/Widgets/Widget_CharacterPreview"));
	if (CustomWidgetBP.Class != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found custom widget achievement class %s !"), *CustomWidgetBP.Class->GetName());

		CharacterPreviewWidgetClass = CustomWidgetBP.Class;
	}
}


FReply UCharacterSelectionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const auto Key = InKeyEvent.GetKey();
	
	if (Key == EKeys::Right)
	{
		RightArrowPressed();
		UpdateBorderDisplay();
		
		return FReply::Handled();
	}

	if (Key == EKeys::Left)
	{
		LeftArrowPressed();
		UpdateBorderDisplay();

		return FReply::Handled();
	}

	if (Key == EKeys::Up)
	{
		UpArrowPressed();
		UpdateBorderDisplay();

		return FReply::Handled();
	}

	if (Key == EKeys::Down)
	{
		DownArrowPressed();
		UpdateBorderDisplay();

		return FReply::Handled();
	}

	if (Key == EKeys::Enter)
	{
		GoToMenu();

		return FReply::Handled();
	}

		return FReply::Unhandled();
}

void UCharacterSelectionWidget::RightArrowPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Right Pressed"))

	if (CurrentIndexCharacterSelected == 4 || CurrentIndexCharacterSelected == 5)
	{
		CurrentIndexCharacterSelected++;
	}
	else if (CurrentIndexCharacterSelected == 6)
	{
		CurrentIndexCharacterSelected = 4;
	}
	else if (CurrentIndexCharacterSelected == 8)
	{
		CurrentIndexCharacterSelected = 6;
	}
}

void UCharacterSelectionWidget::LeftArrowPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Left Pressed"));

	if (CurrentIndexCharacterSelected == 5 || CurrentIndexCharacterSelected == 6)
	{
		CurrentIndexCharacterSelected--;
	}
	else if (CurrentIndexCharacterSelected == 4)
	{
		CurrentIndexCharacterSelected = 6;
	}
	else if (CurrentIndexCharacterSelected == 8)
	{
		CurrentIndexCharacterSelected = 4;
	}
}

void UCharacterSelectionWidget::UpArrowPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Up Pressed"));

	if (CurrentIndexCharacterSelected == 4 || CurrentIndexCharacterSelected == 5 || CurrentIndexCharacterSelected == 6)
	{
		OldIndexCharacterSelected = CurrentIndexCharacterSelected;
		CurrentIndexCharacterSelected = 8;
	}
	else if (CurrentIndexCharacterSelected == 8)
	{
		CurrentIndexCharacterSelected = OldIndexCharacterSelected;
	}
}

void UCharacterSelectionWidget::DownArrowPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Up Pressed"));

	if (CurrentIndexCharacterSelected == 4 || CurrentIndexCharacterSelected == 5 || CurrentIndexCharacterSelected == 6)
	{
		OldIndexCharacterSelected = CurrentIndexCharacterSelected;
		CurrentIndexCharacterSelected = 8;
	}
	else if (CurrentIndexCharacterSelected == 8)
	{
		CurrentIndexCharacterSelected = OldIndexCharacterSelected;
	}
}

void UCharacterSelectionWidget::UpdateBorderDisplay()
{
	if (CurrentIndexCharacterSelected == 4)
	{
		DinoBorder->SetBrushColor(SelectedColor);
		CharacterPreviewWidget->CharacterPreview->SetBrushFromMaterial(DinoPreview);
		HideOtherBorders(4);
	}
	else if (CurrentIndexCharacterSelected == 5)
	{
		PandaBorder->SetBrushColor(SelectedColor);
		CharacterPreviewWidget->CharacterPreview->SetBrushFromMaterial(PandaPreview);
		HideOtherBorders(5);
	}
	else if (CurrentIndexCharacterSelected == 6)
	{
		HyenaBorder->SetBrushColor(SelectedColor);
		CharacterPreviewWidget->CharacterPreview->SetBrushFromMaterial(HyenaPreview);
		HideOtherBorders(6);
	}
	else if (CurrentIndexCharacterSelected == 8)
	{
		CatBorder->SetBrushColor(SelectedColor);
		CharacterPreviewWidget->CharacterPreview->SetBrushFromMaterial(CatPreview);
		HideOtherBorders(8);
	}
}

void UCharacterSelectionWidget::HideOtherBorders(int BorderThatShouldDisplay)
{
	
	if (BorderThatShouldDisplay == 4)
	{
		PandaBorder->SetBrushColor(HideColor);
		HyenaBorder->SetBrushColor(HideColor);
		CatBorder->SetBrushColor(HideColor);
	}
	else if (BorderThatShouldDisplay == 5)
	{
		DinoBorder->SetBrushColor(HideColor);
		HyenaBorder->SetBrushColor(HideColor);
		CatBorder->SetBrushColor(HideColor);
	}
	else if (BorderThatShouldDisplay == 6)
	{
		PandaBorder->SetBrushColor(HideColor);
		DinoBorder->SetBrushColor(HideColor);
		CatBorder->SetBrushColor(HideColor);
	}
	else if (BorderThatShouldDisplay == 8)
	{
		PandaBorder->SetBrushColor(HideColor);
		DinoBorder->SetBrushColor(HideColor);
		HyenaBorder->SetBrushColor(HideColor);
	}
}


