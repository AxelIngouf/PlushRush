// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Input/Reply.h"
#include "CharacterPreviewWidget.h"
#include "CharacterSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UCharacterSelectionWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    //bool Initialize() override;
    UCharacterSelectionWidget(const FObjectInitializer& ObjectInitializer);
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    UFUNCTION()
        void RightArrowPressed();
    UFUNCTION()
        void LeftArrowPressed();
    UFUNCTION()
        void UpArrowPressed();
    UFUNCTION()
        void DownArrowPressed();
    UFUNCTION()
        void UpdateBorderDisplay();
    UFUNCTION()
        void HideOtherBorders(int BorderThatShouldDisplay);

    UFUNCTION(BlueprintImplementableEvent)
        void GoToMenu();

    // Index is 5
    UPROPERTY(meta = (BindWidget))
        UBorder* PandaBorder;

    // Index is 8
    UPROPERTY(meta = (BindWidget))
        UBorder* CatBorder;

    // Index is 6
    UPROPERTY(meta = (BindWidget))
        UBorder* HyenaBorder;

    // Index is 4
    UPROPERTY(meta = (BindWidget))
        UBorder* DinoBorder;

    //4 = Dino, 5 = Panda, 6 = Hyena, 8 = Cat // To Reference use number pad of keyboard comparis to the placement of the characters on the machine
    UPROPERTY(BlueprintReadOnly)
    int CurrentIndexCharacterSelected = 4;

	
    int OldIndexCharacterSelected = 0;

    FLinearColor HideColor = FLinearColor(0.f, 0.f, 0.f, 0.f);


    FLinearColor SelectedColor = FLinearColor(0.3f, 0.f, 0.3f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UCharacterPreviewWidget* CharacterPreviewWidget;

    TSubclassOf<UUserWidget> CharacterPreviewWidgetClass;

    UPROPERTY(EditDefaultsOnly)
        UMaterialInterface* PandaPreview;
    UPROPERTY(EditDefaultsOnly)
        UMaterialInterface* CatPreview;
    UPROPERTY(EditDefaultsOnly)
        UMaterialInterface* HyenaPreview;
    UPROPERTY(EditDefaultsOnly)
        UMaterialInterface* DinoPreview;

};
