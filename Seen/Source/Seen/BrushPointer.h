// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "BrushPointer.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SEEN_API UBrushPointer : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UBrushPointer(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UImage> Pointer;

	UFUNCTION(BlueprintCallable)
	void ChangeColor(UTexture2D* InColor);

	UFUNCTION(BlueprintCallable)
	void ChangeSize(float InSize);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Texture)
	TObjectPtr<class UTexture2D> PointerTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrushSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrushPos;
};
