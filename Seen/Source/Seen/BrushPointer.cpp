// Fill out your copyright notice in the Description page of Project Settings.


#include "BrushPointer.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

UBrushPointer::UBrushPointer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BrushSize = 5.0f;
	BrushPos = -2.5f;
}

void UBrushPointer::ChangeColor(UTexture2D* InColor)
{
	if (Pointer != nullptr && InColor != nullptr)
	{
		PointerTexture = InColor;
		Pointer->SetBrushFromTexture(PointerTexture);
	}
}

void UBrushPointer::ChangeSize(float InSize)
{
	BrushSize += InSize;
	if (BrushSize < 5.0f) BrushSize = 5.0f;
	BrushPos = -(BrushSize/2);

	UWidgetLayoutLibrary::SlotAsCanvasSlot(Pointer)->SetSize(FVector2D(BrushSize, BrushSize));
	UWidgetLayoutLibrary::SlotAsCanvasSlot(Pointer)->SetPosition(FVector2D(BrushPos, BrushPos));
}
