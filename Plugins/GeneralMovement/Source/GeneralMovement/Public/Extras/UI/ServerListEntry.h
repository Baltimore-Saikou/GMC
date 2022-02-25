// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "ServerListEntry.generated.h"

UCLASS()
class GMC_API UServerListEntry : public UUserWidget
{
  GENERATED_BODY()

public:

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  /// Name of the server for this entry.
  class UTextBlock* ServerName;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  /// Username of the host for this server.
  UTextBlock* HostName;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  /// Number of players currently joined.
  UTextBlock* NumCurrentPlayers;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  /// Maximum number of players for this server.
  UTextBlock* NumMaxPlayers;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  /// Ping to the session (not provided by the Steam API).
  UTextBlock* Ping;

  UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidgetOptional))
  /// Makes the server list entry clickable.
  class UButton* ServerButton;

  void SetEntryIndex(UUserWidget* InParent, uint32 InIndex);

private:

  /// Index in the server list for this entry.
  uint32 Index;

  UFUNCTION()
  void OnServerButtonClicked();

  UPROPERTY()
  UUserWidget* Parent;
};
