// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintBase.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APaintBase::APaintBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Drawing/Paint/Sphere.Sphere'"));
	if (SphereMeshRef.Object != nullptr)
	{
		SphereMesh = SphereMeshRef.Object;
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	
	if (SphereMesh != nullptr)
	{
		MeshComp->SetStaticMesh(SphereMesh);
	}

	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void APaintBase::BeginPlay()
{
	Super::BeginPlay();
	
}

UTexture2D* APaintBase::GetColorTexture()
{
	if (ColorTexture == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Paint Texture Empty"));
	}
	return ColorTexture;
}

