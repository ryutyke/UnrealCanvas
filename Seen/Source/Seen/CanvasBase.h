// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CanvasBase.generated.h"

UCLASS()
class SEEN_API ACanvasBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACanvasBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> CanvasComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StaticMesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMesh> CanvasSTMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTextureRenderTarget2D> CanvasRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterial> CanvasMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMaterial> BrushMaterial;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> CanvasMaterialInstance;
	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> BrushMaterialInstance;

	UFUNCTION(BlueprintCallable)
	void DrawBrush(class UTexture2D* InBrushTexture, float InBrushSize, FVector2D InDrawLocation);
};
