// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "MainMenu.generated.h"

UCLASS()
class GMC_API UMainMenu : public UUserWidget
{
  GENERATED_BODY()

public:

  bool Initialize() override;

  /// Menu top-level.

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UWidget* Main;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  class UButton* Main_HostButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Main_JoinButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  class UTextBlock* Main_HostText;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Main_ExitButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  class UWidgetSwitcher* Main_MenuSwitcher;

  UFUNCTION()
  void OnMain_HostButtonClicked();

  UFUNCTION()
  void OnMain_JoinButtonClicked();

  UFUNCTION()
  void OnMain_ExitButtonClicked();

  /// Host submenu.

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UWidget* Host;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Host_HostButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UTextBlock* Host_HostText;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Host_CancelButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Host_SinglePlayerButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Host_MultiplayerButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  class UEditableTextBox* Host_UserDesiredSessionNameTextBox;

  UFUNCTION()
  void OnHost_HostButtonClicked();

  UFUNCTION()
  void OnHost_CancelButtonClicked();

  UFUNCTION()
  void OnHost_SinglePlayerButtonClicked();

  UFUNCTION()
  void OnHost_MultiplayerButtonClicked();

  UFUNCTION()
  void OnHost_UserDesiredSessionNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
  FString UserDesiredSessionName;

  void ResetHostButton();

  /// Join submenu.

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UWidget* Join;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Join_JoinButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UTextBlock* Join_JoinText;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Join_CancelButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  class UScrollBox* Join_ServerListScrollBox;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UButton* Join_RefreshButton;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  UTextBlock* Join_RefreshText;

  UFUNCTION()
  void OnJoin_JoinButtonClicked();

  UFUNCTION()
  void OnJoin_CancelButtonClicked();

  UFUNCTION()
  void OnJoin_RefreshButtonClicked();

  void RefreshServerListUI();

  void ResetJoinButton();

  void SetSelectedServerListIndex(uint32 Index);

  TOptional<uint32> SelectedServerListIndex;

private:

  void SetDefaultButtonStyle(UButton* Button);

  void SetActiveButtonStyle(UButton* Button);
};
