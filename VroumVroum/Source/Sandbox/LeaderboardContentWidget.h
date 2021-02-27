// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "LeaderboardContentWidget.generated.h"

/**
 * 
 */
UCLASS()
class SANDBOX_API ULeaderboardContentWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* Placement;
	UPROPERTY(meta = (BindWidget))
		UImage* CharacterThumbnail;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CharacterName;
	
protected:
	
	
};
