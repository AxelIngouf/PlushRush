// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EndGameWidget.h"
#include "UIDataHolderController.generated.h"


/**
 * 
 */
UCLASS()
class SANDBOX_API AUIDataHolderController : public APlayerController
{
	GENERATED_BODY()

public:
	// Note: that I am using forward declaration Because I am not including the
	// widget in the header and to prevent circular dependency.
	// You don't need to do that if you include the Widget Class in the .h
	// Forward declaration is just putting "class" before the class name so the compiler know it's a
	// class but it's not included in the header and don't freak out. Ex. “class UUserWidget”

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UEndGameWidget> EndGameMainMenu;

	// Variable to hold the widget After Creating it.
	UEndGameWidget* MyEndGameMainMenu;

	// Override BeginPlay()
	virtual void BeginPlay() override;
	
};
