// Copyright Epic Games, Inc. All Rights Reserved.

#include "SeenCharacter.h"
#include "SeenProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CanvasBase.h"
#include "PaintBase.h"
#include "Kismet/GameplayStatics.h"
#include "BrushPointer.h"


//////////////////////////////////////////////////////////////////////////
// ASeenCharacter

ASeenCharacter::ASeenCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	BrushSize = 10.0f;

	static ConstructorHelpers::FClassFinder<UUserWidget> BrushPointerClassRef(TEXT("/Game/Drawing/BrushPointer.BrushPointer_C"));
	if (BrushPointerClassRef.Class)
	{
		BrushPointerClass = BrushPointerClassRef.Class;
	}
	BrushPointer = nullptr;

	static ConstructorHelpers::FObjectFinder<UTexture2D> BrushTextureRef(TEXT("/Script/Engine.Texture2D'/Game/Drawing/Paint/Texture/Black.Black'"));
	if (BrushTextureRef.Object != nullptr)
	{
		BrushTexture = BrushTextureRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> EraseTextureRef(TEXT("/Script/Engine.Texture2D'/Game/Drawing/Paint/Texture/White.White'"));
	if (EraseTextureRef.Object != nullptr)
	{
		EraseTexture = EraseTextureRef.Object;
	}

	static ConstructorHelpers::FClassFinder<ACanvasBase> CanvasClassRef(TEXT("/Game/Drawing/BP_CanvasBase.BP_CanvasBase_C"));
	if (CanvasClassRef.Class)
	{
		CanvasClass = CanvasClassRef.Class;
	}
}

void ASeenCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController != nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		if (BrushPointerClass)
		{
			BrushPointer = CreateWidget<UBrushPointer>(PlayerController, BrushPointerClass);
			if (BrushPointer != nullptr)
			{
				//BrushPointer->AddToViewport();
				BrushPointer->AddToPlayerScreen();
			}
		}
		//BrushPointer->ChangeColor(EraseTexture);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ASeenCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASeenCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASeenCharacter::Look);

		//Painting
		EnhancedInputComponent->BindAction(PaintAction, ETriggerEvent::Triggered, this, &ASeenCharacter::Paint);

		EnhancedInputComponent->BindAction(EraseAction, ETriggerEvent::Triggered, this, &ASeenCharacter::Erase);

		EnhancedInputComponent->BindAction(PaperAction, ETriggerEvent::Triggered, this, &ASeenCharacter::SpawnCanvas);

		EnhancedInputComponent->BindAction(SizeAction, ETriggerEvent::Triggered, this, &ASeenCharacter::ChangeSize);
	}
}

void ASeenCharacter::SpawnCanvas()
{
	if (CanvasClass)
	{
		FVector Location = GetActorLocation() + GetActorForwardVector() * 400.0f;
		Location.Z = 170.0;
		FRotator Rotation = GetActorRotation();

		AActor* SpawnedActor = GetWorld()->SpawnActor<ACanvasBase>(CanvasClass, Location, Rotation);
	}
}

void ASeenCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASeenCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASeenCharacter::Paint()
{
	FHitResult HitResult;
	static const FName LineTraceSingleName(TEXT("LineTraceSingle"));
	FCollisionQueryParams Params(LineTraceSingleName);
	Params.bTraceComplex = true;
	Params.bReturnFaceIndex = true;
	
	const AActor* IgnoreActor = Cast<AActor>(this);
	if (IgnoreActor)
	{
		Params.AddIgnoredActor(IgnoreActor);
	}

	APlayerCameraManager* PlayerCamera = CastChecked<APlayerController>(GetController())->PlayerCameraManager;

	FVector Start = PlayerCamera->GetCameraLocation();
	FVector End = PlayerCamera->GetCameraLocation() + (PlayerCamera->GetActorForwardVector() * 1500.0f);

	bool HitDetected = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	if (HitDetected)
	{
		if (APaintBase* PaintActor = Cast<APaintBase>(HitResult.GetActor()))
		{
			UTexture2D* ColorTexture = PaintActor->GetColorTexture();
			if (ColorTexture != nullptr)
			{
				BrushPointer->ChangeColor(ColorTexture);
				BrushTexture = ColorTexture;
			}
		}

		else if (ACanvasBase* CanvasActor = Cast<ACanvasBase>(HitResult.GetActor()))
		{
			FVector2D HitUV;
			if (BrushTexture != nullptr && UGameplayStatics::FindCollisionUV(HitResult, 0, HitUV))
			{
				CanvasActor->DrawBrush(BrushTexture, BrushSize, HitUV);
			}
		}
	}
}

void ASeenCharacter::Erase()
{
	FHitResult HitResult;
	static const FName LineTraceSingleName(TEXT("LineTraceSingle"));
	FCollisionQueryParams Params(LineTraceSingleName);
	Params.bTraceComplex = true;
	Params.bReturnFaceIndex = true;

	const AActor* IgnoreActor = Cast<AActor>(this);
	if (IgnoreActor)
	{
		Params.AddIgnoredActor(IgnoreActor);
	}

	APlayerCameraManager* PlayerCamera = CastChecked<APlayerController>(GetController())->PlayerCameraManager;

	FVector Start = PlayerCamera->GetCameraLocation();
	FVector End = PlayerCamera->GetCameraLocation() + (PlayerCamera->GetActorForwardVector() * 1500.0f);

	bool HitDetected = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	if (HitDetected && EraseTexture != nullptr)
	{
		FVector2D HitUV;
		if (UGameplayStatics::FindCollisionUV(HitResult, 0, HitUV))
		{
			if (ACanvasBase* CanvasActor = Cast<ACanvasBase>(HitResult.GetActor()))
			{
				CanvasActor->DrawBrush(EraseTexture, BrushSize, HitUV);
			}
		}
	}
}

void ASeenCharacter::ChangeSize(const FInputActionValue& Value)
{
	BrushPointer->ChangeSize(Value.GetMagnitude());
	BrushSize += Value.GetMagnitude();
	if (BrushSize < 10.0f) BrushSize = 10.0f;
}

void ASeenCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ASeenCharacter::GetHasRifle()
{
	return bHasRifle;
}