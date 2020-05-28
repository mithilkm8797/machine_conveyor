// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "VRSimulator.h"
#include "GameFramework/InputSettings.h"

#define LOCTEXT_NAMESPACE "FVRSimulatorModule"

void FVRSimulatorModule::AddActionIfNotDefined(UInputSettings* Settings, TArray<FInputActionKeyMapping> DefinedActions, FInputActionKeyMapping NewAction)
{
	for(FInputActionKeyMapping Action : DefinedActions)
	{
		if(Action.ActionName == NewAction.ActionName) return; //Dont Add
	}
	Settings->AddActionMapping(NewAction);
}

void FVRSimulatorModule::AddAxisIfNotDefined(UInputSettings* Settings, TArray<FInputAxisKeyMapping> DefinedAxis, FInputAxisKeyMapping NewAxis)
{
	for(FInputAxisKeyMapping Action : DefinedAxis)
	{
		if(Action.AxisName == NewAxis.AxisName) return; //Dont Add
	}
	Settings->AddAxisMapping(NewAxis);
}


void FVRSimulatorModule::StartupModule()
{
	UInputSettings* Settings = UInputSettings::GetInputSettings();
	const TArray<FInputActionKeyMapping> DefinedActions(Settings->ActionMappings);
	const TArray<FInputAxisKeyMapping> DefinedAxis(Settings->AxisMappings);
	
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorCycleMovementMode"),EKeys::Tab));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorMovementModeCamera"),EKeys::One));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorMovementModeLeftHand"),EKeys::Two));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorMovementModeRightHand"),EKeys::Three));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorTrigger"),EKeys::T));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorTrigger"),EKeys::LeftMouseButton));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorGrip"),EKeys::G));
	AddActionIfNotDefined(Settings, DefinedActions, FInputActionKeyMapping(TEXT("VRSimulatorMenu"),EKeys::M));

	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorX"), EKeys::MouseX));
	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorY"), EKeys::MouseY));
	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorZ"), EKeys::MouseWheelAxis));
	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorForward"), EKeys::W, 1));
	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorForward"), EKeys::S, -1));
	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorRight"), EKeys::D, 1));
	AddAxisIfNotDefined(Settings, DefinedAxis, FInputAxisKeyMapping(TEXT("VRSimulatorRight"), EKeys::A, -1));
	
	//Save Updates
	Settings->SaveKeyMappings();
}

void FVRSimulatorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVRSimulatorModule, VRSimulator)
