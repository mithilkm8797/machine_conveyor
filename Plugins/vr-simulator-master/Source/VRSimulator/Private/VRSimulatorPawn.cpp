// Fill out your copyright notice in the Description page of Project Settings.

#include "VRSimulatorPawn.h"

#include "ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogVRSimulator);

template <typename T>
bool LoadAsset(const FString& Path, T* & Result)
{
	ConstructorHelpers::FObjectFinder<T> Loader(*Path);
	Result = Loader.Object;
	if (!Loader.Succeeded()) UE_LOG(LogVRSimulator, Error, TEXT("Could not find %s. Have you renamed it?"), *Path);
	return Loader.Succeeded();
}

FVector CartesianToPolar(FVector InCartesian)
{
	const float Radius = InCartesian.Size();
	return FVector(
		FMath::Acos(InCartesian.X / FMath::Sqrt(InCartesian.X * InCartesian.X + InCartesian.Y * InCartesian.Y)) * ((InCartesian.Y >= 0) ? 1.0f : -1.0f),
		FMath::Asin(InCartesian.Z / Radius),
		Radius
	);
}

FVector PolarToCartesian(FVector& InPolar) //Azimuth (Longitude), Elevation (Latitude), Radius
{
	return FVector(
		FMath::Cos(InPolar.Y) * FMath::Cos(InPolar.X) * InPolar.Z,
		FMath::Cos(InPolar.Y) * FMath::Sin(InPolar.X) * InPolar.Z,
		FMath::Sin(InPolar.Y) * InPolar.Z
	);
}

// Sets default values
AVRSimulatorPawn::AVRSimulatorPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LoadAsset("/VRSimulator/ControllerModel", ControllerMesh);
	LoadAsset("/VRSimulator/ControllerMaterial", ControllerMaterial);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionController"));
	LeftMotionController->SetupAttachment(Root);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);
	LeftMotionController->SetShowDeviceModel(false);
	LeftController.ControllerComponent = LeftMotionController;

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionController"));
	RightMotionController->SetupAttachment(Root);
	RightMotionController->SetTrackingSource(EControllerHand::Right);
	RightMotionController->SetShowDeviceModel(false);
	RightController.ControllerComponent = RightMotionController;

	LeftControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftControllerMesh");
	LeftControllerMesh->SetStaticMesh(ControllerMesh);
	LeftControllerMesh->SetupAttachment(LeftMotionController);
	LeftController.ControllerMesh = LeftControllerMesh;

	RightControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>("RightControllerMesh");
	RightControllerMesh->SetStaticMesh(ControllerMesh);
	RightControllerMesh->SetupAttachment(RightMotionController);
	RightController.ControllerMesh = RightControllerMesh;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Root);
	Camera->bUsePawnControlRotation = true;

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->SetUpdatedComponent(Root);
}

// Called when the game starts or when spawned
void AVRSimulatorPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());
	RightController.AccumulatedPosition = CartesianToPolar(RightController.ControllerComponent->GetRelativeTransform().GetLocation());
	RightController.AccumulatedRotation = RightController.ControllerComponent->GetRelativeTransform().GetRotation().Euler();
	LeftController.AccumulatedPosition = CartesianToPolar(LeftController.ControllerComponent->GetRelativeTransform().GetLocation());
	LeftController.AccumulatedRotation = LeftController.ControllerComponent->GetRelativeTransform().GetRotation().Euler();
}

// Called every frame
void AVRSimulatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRSimulatorPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LeftController.ControllerMaterialDynamic = UMaterialInstanceDynamic::Create(ControllerMaterial, LeftController.ControllerMesh);
	RightController.ControllerMaterialDynamic = UMaterialInstanceDynamic::Create(ControllerMaterial, RightController.ControllerMesh);
	LeftController.ControllerMesh->SetMaterial(0, LeftController.ControllerMaterialDynamic);
	RightController.ControllerMesh->SetMaterial(0, RightController.ControllerMaterialDynamic);
}

// Called to bind functionality to input
void AVRSimulatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Buttons
	PlayerInputComponent->BindAction<FSwitchModeDelegate>(TEXT("VRSimulatorCycleMovementMode"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputMode, ESimulatorMovementMode::Cycle_Next);
	PlayerInputComponent->BindAction<FSwitchModeDelegate>(TEXT("VRSimulatorMovementModeCamera"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputMode, ESimulatorMovementMode::Camera);
	PlayerInputComponent->BindAction<FSwitchModeDelegate>(TEXT("VRSimulatorMovementModeLeftHand"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputMode, ESimulatorMovementMode::LeftHand);
	PlayerInputComponent->BindAction<FSwitchModeDelegate>(TEXT("VRSimulatorMovementModeRightHand"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputMode, ESimulatorMovementMode::RightHand);

	PlayerInputComponent->BindAction<FControllerButtonDelegate>(TEXT("VRSimulatorTrigger"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputTrigger, true);
	PlayerInputComponent->BindAction<FControllerButtonDelegate>(TEXT("VRSimulatorTrigger"), EInputEvent::IE_Released, this, &AVRSimulatorPawn::InputTrigger, false);
	PlayerInputComponent->BindAction<FControllerButtonDelegate>(TEXT("VRSimulatorGrip"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputGrip, true);
	PlayerInputComponent->BindAction<FControllerButtonDelegate>(TEXT("VRSimulatorGrip"), EInputEvent::IE_Released, this, &AVRSimulatorPawn::InputGrip, false);
	PlayerInputComponent->BindAction<FControllerButtonDelegate>(TEXT("VRSimulatorMenu"), EInputEvent::IE_Pressed, this, &AVRSimulatorPawn::InputMenu, true);
	PlayerInputComponent->BindAction<FControllerButtonDelegate>(TEXT("VRSimulatorMenu"), EInputEvent::IE_Released, this, &AVRSimulatorPawn::InputMenu, false);

	//Axis
	PlayerInputComponent->BindAxis(TEXT("VRSimulatorX"), this, &AVRSimulatorPawn::InputX);
	PlayerInputComponent->BindAxis(TEXT("VRSimulatorY"), this, &AVRSimulatorPawn::InputY);
	PlayerInputComponent->BindAxis(TEXT("VRSimulatorZ"), this, &AVRSimulatorPawn::InputZ);

	PlayerInputComponent->BindAxis(TEXT("VRSimulatorForward"), this, &AVRSimulatorPawn::InputMoveForward);
	PlayerInputComponent->BindAxis(TEXT("VRSimulatorRight"), this, &AVRSimulatorPawn::InputMoveRight);
}

void AVRSimulatorPawn::InputMode(ESimulatorMovementMode NewMode)
{
	//Cycle to next one
	if (NewMode == ESimulatorMovementMode::Cycle_Next) NewMode = static_cast<ESimulatorMovementMode>((static_cast<uint8>(CurrentMode) + 1) % static_cast<uint8>(ESimulatorMovementMode::NUM_TYPES));

	switch (NewMode)
	{
	case ESimulatorMovementMode::Camera:
		CurrentController = nullptr;
		LeftController.ControllerMaterialDynamic->SetVectorParameterValue("TrimGlowColor", FColor::Cyan);
		RightController.ControllerMaterialDynamic->SetVectorParameterValue("TrimGlowColor", FColor::Cyan);
		break;
	case ESimulatorMovementMode::LeftHand:
		CurrentController = &LeftController;
		LeftController.ControllerMaterialDynamic->SetVectorParameterValue("TrimGlowColor", FColor::Red);
		RightController.ControllerMaterialDynamic->SetVectorParameterValue("TrimGlowColor", FColor::Cyan);
		break;
	case ESimulatorMovementMode::RightHand:
		CurrentController = &RightController;
		RightController.ControllerMaterialDynamic->SetVectorParameterValue("TrimGlowColor", FColor::Red);
		LeftController.ControllerMaterialDynamic->SetVectorParameterValue("TrimGlowColor", FColor::Cyan);
		break;
	default: return;
	}
	CurrentMode = NewMode;
}

void AVRSimulatorPawn::InputX(float Delta) { if (Delta != 0.0f) InputXYZ(Delta, 0, 0); }
void AVRSimulatorPawn::InputY(float Delta) { if (Delta != 0.0f) InputXYZ(0, Delta, 0); }
void AVRSimulatorPawn::InputZ(float Delta) { if (Delta != 0.0f) InputXYZ(0, 0, Delta); }

void AVRSimulatorPawn::InputTrigger(bool Pressed)
{
	if (CurrentMode == ESimulatorMovementMode::Camera) return; //Not our business
	if (CurrentMode == ESimulatorMovementMode::LeftHand) LeftHandTrigger(Pressed);
	if (CurrentMode == ESimulatorMovementMode::RightHand) RightHandTrigger(Pressed);
}

void AVRSimulatorPawn::InputGrip(bool Pressed)
{
	if (CurrentMode == ESimulatorMovementMode::Camera) return; //Not our business
	if (CurrentMode == ESimulatorMovementMode::LeftHand) LeftHandGrip(Pressed);
	if (CurrentMode == ESimulatorMovementMode::RightHand) RightHandGrip(Pressed);
}

void AVRSimulatorPawn::InputMenu(bool Pressed)
{
	if (CurrentMode == ESimulatorMovementMode::Camera) return; //Not our business
	if (CurrentMode == ESimulatorMovementMode::LeftHand) LeftHandMenu(Pressed);
	if (CurrentMode == ESimulatorMovementMode::RightHand) RightHandMenu(Pressed);
}


void AVRSimulatorPawn::InputXYZ(float DeltaX, float DeltaY, float DeltaZ)
{
	if (CurrentMode == ESimulatorMovementMode::Camera)
	{
		//Move Camera
		if (DeltaX != 0.0f) AddControllerYawInput(DeltaX * 45.0f * GetWorld()->GetDeltaSeconds());
		if (DeltaY != 0.0f) AddControllerPitchInput(-DeltaY * 45.0f * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		if (PlayerController->IsInputKeyDown(EKeys::RightMouseButton)) // Rotate
		{
			CurrentController->AccumulatedRotation += FVector(DeltaZ, -DeltaY, DeltaX) * FVector(1.0f, 1.0f, 2.0f);
			CurrentController->ControllerComponent->SetRelativeRotation(FRotator::MakeFromEuler(CurrentController->AccumulatedRotation));
		}
		else //Move
		{
			CurrentController->AccumulatedPosition += FVector(DeltaX, DeltaY, DeltaZ) * FVector(0.01f, 0.01f, 1.0f);
			CurrentController->ControllerComponent->SetRelativeLocation(PolarToCartesian(CurrentController->AccumulatedPosition));
		}
	}
}

void AVRSimulatorPawn::InputMoveRight(float Delta)
{
	if (CurrentMode == ESimulatorMovementMode::Camera)
	{
		AddMovementInput(Camera->GetRightVector(), Delta * GetWorld()->GetDeltaSeconds() * 20);
	}
}

void AVRSimulatorPawn::InputMoveForward(float Delta)
{
	if (CurrentMode == ESimulatorMovementMode::Camera)
	{
		AddMovementInput(Camera->GetForwardVector(), Delta * GetWorld()->GetDeltaSeconds() * 20);
	}
}

//Used for Blueprint Events
void AVRSimulatorPawn::RightHandTrigger_Implementation(const bool Pressed)
{
}

void AVRSimulatorPawn::RightHandGrip_Implementation(const bool Pressed)
{
}

void AVRSimulatorPawn::RightHandMenu_Implementation(const bool Pressed)
{
}

void AVRSimulatorPawn::LeftHandTrigger_Implementation(const bool Pressed)
{
}

void AVRSimulatorPawn::LeftHandGrip_Implementation(const bool Pressed)
{
}

void AVRSimulatorPawn::LeftHandMenu_Implementation(const bool Pressed)
{
}
