// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "CharacterPreviewWidget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API UCharacterPreviewWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(meta = (BindWidget))
        UImage* CharacterPreview;

};
