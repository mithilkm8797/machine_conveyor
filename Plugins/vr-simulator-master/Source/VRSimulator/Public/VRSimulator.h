// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
#include "Modules/ModuleManager.h"

class FVRSimulatorModule : public IModuleInterface
{
public:

	static void AddActionIfNotDefined(UInputSettings* Settings, TArray<FInputActionKeyMapping> DefinedActions, FInputActionKeyMapping NewAction);
	static void AddAxisIfNotDefined(UInputSettings* Settings, TArray<FInputAxisKeyMapping> DefinedAxis, FInputAxisKeyMapping NewAxis);
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
