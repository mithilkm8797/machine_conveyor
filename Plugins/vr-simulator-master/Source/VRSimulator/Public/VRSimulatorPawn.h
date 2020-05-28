// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/InputComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "VRSimulatorPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVRSimulator, Log, All);

/* Hold current mode */
UENUM( BlueprintType )
enum class ESimulatorMovementMode : uint8
{
	LeftHand = 0,
	RightHand = 1,
	Camera = 2,
	NUM_TYPES = 3 UMETA( Hidden ),
	Cycle_Next = 4 UMETA( Hidden )
};

/* Hold Controller Data */
USTRUCT()
struct FControllerData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere) ESimulatorMovementMode ControllerSide;
	UPROPERTY(VisibleAnywhere) UMaterialInstanceDynamic* ControllerMaterialDynamic;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* ControllerMesh;
	UPROPERTY(VisibleAnywhere) UMotionControllerComponent* ControllerComponent;

	UPROPERTY() FVector AccumulatedPosition = FVector::ZeroVector; //Azimuth, Elevation, Radius (Polar Coordinates)
	UPROPERTY() FVector AccumulatedRotation = FVector::ZeroVector; //Roll, Pitch, Yaw
};

UCLASS()
class AVRSimulatorPawn : public APawn
{
	GENERATED_BODY()

public:
	AVRSimulatorPawn();

	/* Called on play */
	virtual void BeginPlay() override;

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

	/* Create Dynamic Materials here */
	virtual void PostInitializeComponents() override;

	/* Bind all Input Events here */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Components of the actual Pawn. All visible and editable from blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMotionControllerComponent* LeftMotionController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMotionControllerComponent* RightMotionController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USceneComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UFloatingPawnMovement* Movement = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* LeftControllerMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* RightControllerMesh;

	/* Events that are visible inside the blueprints */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RightHandTrigger(const bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RightHandGrip(const bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void RightHandMenu(const bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LeftHandTrigger(const bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LeftHandGrip(const bool Pressed);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void LeftHandMenu(const bool Pressed);

private:

	/* Input related stuff */
	DECLARE_DELEGATE_OneParam(FSwitchModeDelegate, ESimulatorMovementMode);
	DECLARE_DELEGATE_OneParam(FControllerButtonDelegate, bool);

	UFUNCTION() void InputMode(ESimulatorMovementMode NewMode);
	UFUNCTION() void InputX(float Delta);
	UFUNCTION() void InputY(float Delta);
	UFUNCTION() void InputZ(float Delta);
	UFUNCTION() void InputTrigger(bool Pressed);
	UFUNCTION() void InputGrip(bool Pressed);
	UFUNCTION() void InputMenu(bool Pressed);
	UFUNCTION() void InputMoveRight(float Delta);
	UFUNCTION() void InputMoveForward(float Delta);
	UFUNCTION() void InputXYZ(float DeltaX = 0, float DeltaY = 0, float DeltaZ = 0 );

	/* Controller and movement related data */
	UPROPERTY() FControllerData LeftController;
	UPROPERTY() FControllerData RightController;
	FControllerData* CurrentController = nullptr; //Can't be UPROPERTY
	UPROPERTY() ESimulatorMovementMode CurrentMode = ESimulatorMovementMode::Camera;

	/* Cached variables and references */
	UPROPERTY() UStaticMesh* ControllerMesh = nullptr;
	UPROPERTY() UMaterial* ControllerMaterial = nullptr;
	UPROPERTY() APlayerController* PlayerController = nullptr;
};
