// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "GenPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGMCController, Log, All);

/// Controller class intended to be used with @see UGenMovementReplicationComponent.
UCLASS(BlueprintType, Blueprintable)
class GMC_API AGenPlayerController : public APlayerController
{
  GENERATED_BODY()

public:

  AGenPlayerController();
  void BeginPlay() override;
  void PlayerTick(float DeltaTime) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma region Console Commands

  UFUNCTION(Exec)
  /// Set the interpolation delay for remotely controlled pawns on a local machine.
  void SetInterpolationDelay(float Delay) const;
  UFUNCTION(Exec)
  /// Set the interpolation method for remotely controlled pawns on a local machine (@see EInterpolationMethod).
  void SetInterpolationMethod(int32 Method) const;
  UFUNCTION(Exec)
  /// Set whether extrapolation is allowed for remotely controlled pawns on a local machine when no recent enough states available.
  void SetExtrapolationAllowed(bool bAllowed) const;
  UFUNCTION(Exec)
  /// Set whether the root collision of remotely controlled pawns on a local machine should be smoothed.
  void SetSmoothCollision(bool bSmoothLocation, bool bSmoothRotation) const;

#pragma endregion

#pragma region Client Time Syncronisation

  /// Returns the synchronised server world time for a client. Considers network delay, meaning that the returned value will be nearly the
  /// same as the world time on the server at any given moment.
  ///
  /// @returns      float    The current synchronised server world time.
  float Client_GetSyncedWorldTimeSeconds() const;

private:

  /// The time in seconds since the client world was brought up. Synchronised with the server time periodically.
  float Client_SyncedWorldTime{0.f};

  /// The timer handle for world time syncing.
  FTimerHandle Client_TimeSyncHandle;

  /// What the time was during the last frame so we can identify inconsistent timestamps (can happen after the time was just synced).
  float Client_SyncedTimeLastFrame{0.f};

  /// If set only half the delta time will be added to the client world time during the next update (@see Client_UpdateWorldTime). This is
  /// done to preemptively bring the client time closer to the server time, as a correction back in time could force the client to discard
  /// moves.
  bool Client_bSlowWorldTime{false};

  /// Tracks the client world time by adding the current delta time to the world time every frame. The client time is synced periodically
  /// with the authoritative server world time (@see Client_SyncWithServerTime).
  ///
  /// @param        DeltaTime    The current delta time.
  /// @returns      void
  void Client_UpdateWorldTime(float DeltaTime);

  /// Queries and synchronises the client world time with the server world time if necessary (accounts for network delay). The time will
  /// only be synchronised if the deviation to the server world time is greater than @see MaxClientTimeDifference.
  ///
  /// @returns      void
  void Client_SyncWithServerTime();

protected:

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Client Time Sync", meta =
    (ClampMin = "0.1", UIMin = "1", UIMax = "60"))
  /// The interval in seconds at which the client should query the server world time. The client time will not be synchronised unless the
  /// difference exceeds the set threshold.
  float TimeSyncInterval{5.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Client Time Sync", meta =
    (ClampMin = "0", UIMin = "0", UIMax = "1"))
  /// The maximum acceptable difference in seconds between the local client world time and the authoritative server world time. Syncing
  /// i.e. overwriting the local time with the server time can create inconsistencies for consecutive timestamps which can cause stutter
  /// for the client. If the difference is smaller or equal to this value the time will not be synced.
  float MaxClientTimeDifference{0.01f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Client Time Sync", meta =
    (ClampMin = "0", UIMin = "0", UIMax = "60"))
  /// The delay before the first synchronisation with the server world time in seconds. Gives the net connection object some time to
  /// calculate an accurate average round trip time value.
  float InitialTimeSyncDelay{1.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Client Time Sync", meta =
    (ClampMin = "0", ClampMax = "1", UIMin = "0.05", UIMax = "1"))
  /// The max ping that a client is expected to have (in seconds). This is not enforced but if a client has a higher ping than this the
  /// local world time will desync which can create all sorts of problems. Set this to the lowest value that is acceptable.
  float MaxExpectedPing{0.5f};

#pragma endregion
};
