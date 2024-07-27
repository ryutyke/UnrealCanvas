// Fill out your copyright notice in the Description page of Project Settings.


#include "CanvasBase.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Engine/Canvas.h"


// Sets default values
ACanvasBase::ACanvasBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CanvasSTMeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (CanvasSTMeshRef.Object != nullptr)
	{
		CanvasSTMesh = CanvasSTMeshRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> CanvasMaterialRef(TEXT("/Script/Engine.Material'/Game/Drawing/M_Canvas.M_Canvas'"));
	if (CanvasMaterialRef.Object != nullptr)
	{
		CanvasMaterial = CanvasMaterialRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> BrushMaterialRef(TEXT("/Script/Engine.Material'/Game/Drawing/M_Brush.M_Brush'"));
	if (BrushMaterialRef.Object != nullptr)
	{
		BrushMaterial = BrushMaterialRef.Object;
	}

	CanvasComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Canvas"));

	if (CanvasSTMesh != nullptr)
	{
		CanvasComponent->SetStaticMesh(CanvasSTMesh);
	}

	RootComponent = CanvasComponent;

	CanvasComponent->SetWorldRotation(FRotator(90.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void ACanvasBase::BeginPlay()
{
	Super::BeginPlay();
	
	CanvasRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, CanvasRenderTarget, FLinearColor::White);
	CanvasMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, CanvasMaterial);
	CanvasMaterialInstance->SetTextureParameterValue("Paper", CanvasRenderTarget);
	CanvasComponent->SetMaterial(0, CanvasMaterialInstance);

	BrushMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, BrushMaterial);
	
	
}

void ACanvasBase::DrawBrush(UTexture2D* InBrushTexture, float InBrushSize, FVector2D InDrawLocation)
{
	BrushMaterialInstance->SetTextureParameterValue("Brush", InBrushTexture);

	UCanvas* Canvas = nullptr;
	FVector2D CanvasSize;
	FDrawToRenderTargetContext RenderContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, CanvasRenderTarget, Canvas, CanvasSize, RenderContext);

	Canvas->K2_DrawMaterial(BrushMaterialInstance, (CanvasSize * InDrawLocation) - (InBrushSize / 2), FVector2D(InBrushSize, InBrushSize), FVector2D(0, 0));

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, RenderContext);
}

