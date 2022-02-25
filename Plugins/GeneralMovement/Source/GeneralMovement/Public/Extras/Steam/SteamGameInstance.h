// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "SteamGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGMCOnlineSubsystem, Log, All);

DECLARE_DELEGATE(FOnCreateSessionCompleted)
DECLARE_DELEGATE(FOnJoinSessionCompleted)
DECLARE_DELEGATE(FOnServerListUpdated)

USTRUCT(BlueprintType)
struct GMC_API FServerInfo
{
  GENERATED_BODY()

  FOnlineSessionSearchResult SessionSearchResult;
  FString ServerName;
  FString HostName;
  int32 NumMaxPlayers;
  int32 NumCurrentPlayers;
  int32 Ping;
};

UCLASS()
class GMC_API USteamGameInstance : public UGameInstance
{
  GENERATED_BODY()

public:

  void Init() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
  /// Project-internal path (no leading slash) to the map that should be hosted when the user launches a game through the menu. The map will
  /// always be hosted as a listen server by default.
  FString HostURL{""};

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session", meta = (ClampMin = "0", UIMin = "0", UIMax = "16"))
  /// The maximum number of connections to accept for a session.
  int32 MaxPublicConnections{16};

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
  /// The main menu class to use.
  TSubclassOf<class UMainMenu> MainMenu;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
  /// The pause menu class to use.
  TSubclassOf<class UPauseMenu> PauseMenu;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
  /// The server list entry class to use.
  TSubclassOf<class UServerListEntry> ServerListEntry;

#pragma region Online Subsystem

  UFUNCTION(BlueprintCallable, Category = "Session")
  /// Creates a session with the passed name.
  bool HostGameSession(const FString& UserDesiredSessionName);

  UFUNCTION(BlueprintCallable, Category = "Session")
  /// Joins the session with the passed index from the server list.
  bool JoinGameSession(int32 ServerEntryIndex);

  UFUNCTION(BlueprintCallable, Category = "Session")
  /// Searches for sessions and updates the server list.
  bool RefreshServerList();

  UFUNCTION(BlueprintCallable, Category = "Session")
  /// Returns the name of the map the player is currently in (without path, file extension or streaming level prefix).
  FString GetCurrentMapName() const;

  /// Use SteamWorks as online subsystem, and NULL as a fallback when Steam is not running (also used in the editor).
  class IOnlineSubsystem* OnlineSubsystem;
  IOnlineSessionPtr SessionInterface;

  /// The settings of a created session.
  FOnlineSessionSettings SessionSettings;

  UPROPERTY(BlueprintReadOnly, Category = "Session")
  /// List of all sessions found.
  TArray<FServerInfo> ServerList;

  UPROPERTY(BlueprintReadOnly, Category = "Session")
  /// User-entered name of the online session.
  FString UserDesiredSessionName;

  /// Using this as session label is a workaround for a bug in the engine when the NULL subsystem is used. The number of open connection
  /// does not get decremented because the default name is always used instead of the user given name.
  /// @see APlayerState::RegisterPlayerWithSession
  FName SessionLabel{NAME_GameSession};

  /// The search parameters for finding sessions.
  TSharedPtr<class FOnlineSessionSearch> SessionSearch;

  /// Session functions are asynchronous processes and not finished until the delegates are called.
  void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful) const;
  void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful) const;
  void OnFindSessionsComplete(bool bWasSuccessful);
  void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type ResultType) const;
  FOnCreateSessionCompleted OnCreateSessionCompleted;
  FOnJoinSessionCompleted OnJoinSessionCompleted;
  FOnServerListUpdated OnServerListUpdated;

  /// Called when hosting or joining fails for any reason.
  UFUNCTION()
  void OnTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString) const;

  /// Called when any kind of network error occurs.
  UFUNCTION()
  void OnNetworkFailure(UWorld* World, class UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

#pragma endregion
};
