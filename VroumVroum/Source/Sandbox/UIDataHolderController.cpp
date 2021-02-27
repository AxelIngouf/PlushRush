// Fill out your copyright notice in the Description page of Project Settings.


#include "UIDataHolderController.h"

#include "Blueprint/UserWidget.h"

void AUIDataHolderController::BeginPlay()
{
	Super::BeginPlay();

	if (EndGameMainMenu) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		MyEndGameMainMenu = CreateWidget<UEndGameWidget>(this, EndGameMainMenu);

		// Now we can use the widget directly since we have a reference for it.
		// Extra check to  make sure the pointer holds the widget.
		if (MyEndGameMainMenu)
		{
			//Add it to the view port
			MyEndGameMainMenu->AddToViewport();
		}

		//Show the Cursor.
		bShowMouseCursor = true;
	}

}