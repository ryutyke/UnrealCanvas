// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintBase.generated.h"

UCLASS()
class SEEN_API APaintBase : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = StaticMesh)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "StaticMesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMesh> SphereMesh;

public:	
	// Sets default values for this actor's properties
	APaintBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = Color)
	TObjectPtr<class UTexture2D> ColorTexture;

	UFUNCTION(BlueprintCallable, Category = Color)
	UTexture2D* GetColorTexture();

};
