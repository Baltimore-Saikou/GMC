// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "PauseMenu.generated.h"

UCLASS()
class GMC_API UPauseMenu : public UUserWidget
{
  GENERATED_BODY()

protected:

  bool Initialize() override;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  class UButton* ExitButton;

  UFUNCTION()
  void OnExitButtonClicked();
};
