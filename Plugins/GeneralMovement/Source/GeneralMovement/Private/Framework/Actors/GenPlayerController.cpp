// Copyright 2022 Dominik Scherer. All Rights Reserved.

#include "GenPlayerController.h"
#include "GenPawn.h"
#include "GenMovementReplicationComponent.h"
#define GMC_CONTROLLER_LOG
#include "GMC_LOG.h"
#include "GenPlayerController_DBG.h"

DEFINE_LOG_CATEGORY(LogGMCController)

namespace GMCCVars
{
#if ALLOW_CONSOLE && !NO_LOGGING

  int32 StatPing = 0;
  FAutoConsoleVariableRef CVarStatPing(
    TEXT("gmc.StatPing"),
    StatPing,
    TEXT("Display the current ping of the local machine to the server. Only applicable on clients. 0: Disable, 1: Enable"),
    ECVF_Default
  );

  int32 LogNetWorldTime = 0;
  FAutoConsoleVariableRef CVarLogNetWorldTime(
    TEXT("gmc.LogNetWorldTime"),
    LogNetWorldTime,
    TEXT("Log the current world time on server and client with a UTC-timestamp. 0: Disable, 1: Enable"),
    ECVF_Default
  );

#endif
}

AGenPlayerController::AGenPlayerController()
{
  bAllowTickBeforeBeginPlay = false;
  bReplicates = true;
}

void AGenPlayerController::BeginPlay()
{
  Super::BeginPlay();

  if (GetLocalRole() == ROLE_AutonomousProxy)
  {
    if (const auto World = GetWorld())
    {
      World->GetTimerManager().SetTimer(
        Client_TimeSyncHandle,
        this,
        &AGenPlayerController::Client_SyncWithServerTime,
        TimeSyncInterval,
        true,
        InitialTimeSyncDelay
      );
    }
  }
}

void AGenPlayerController::PlayerTick(float DeltaTime)
{
  Super::PlayerTick(DeltaTime);
  DEBUG_STAT_PING
  DEBUG_LOG_NET_WORLD_TIME

  if (GetLocalRole() == ROLE_AutonomousProxy)
  {
    Client_UpdateWorldTime(DeltaTime);
  }
}

void AGenPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  // Clear timers when this object gets destroyed.
  if (GetLocalRole() == ROLE_AutonomousProxy)
  {
    if (const auto World = GetWorld()) World->GetTimerManager().ClearTimer(Client_TimeSyncHandle);
  }
}

float AGenPlayerController::Client_GetSyncedWorldTimeSeconds() const
{
  checkGMC(GetLocalRole() < ROLE_Authority)
  return Client_SyncedWorldTime;
}

void AGenPlayerController::Client_UpdateWorldTime(float DeltaTime)
{
  checkGMC(GetLocalRole() == ROLE_AutonomousProxy)

  if (Client_bSlowWorldTime)
  {
    // Only add half the delta time to bring the client time closer to the server time. This will effectively slow down movement for one
    // frame (which is usually imperceptible).
    Client_SyncedWorldTime += DeltaTime / 2.f;
    Client_bSlowWorldTime = false;
    GMC_LOG(Verbose, TEXT("Client world time was slowed down for this frame."))
  }
  else
  {
    Client_SyncedWorldTime += DeltaTime;
  }

  GMC_CLOG(
    Client_SyncedWorldTime <= Client_SyncedTimeLastFrame,
    VeryVerbose,
    TEXT("Client has world time inconsistency: Timestamp current frame (%9.3f) <= Timestamp last frame (%9.3f)"),
    Client_SyncedWorldTime,
    Client_SyncedTimeLastFrame
  )

  Client_SyncedTimeLastFrame = Client_SyncedWorldTime;
}

void AGenPlayerController::Client_SyncWithServerTime()
{
  checkGMC(GetLocalRole() == ROLE_AutonomousProxy)

  if (const auto World = GetWorld())
  {
    if (const auto GameState = World->GetGameState())
    {
      const float ServerWorldTime = GameState->GetServerWorldTimeSeconds();
      if (const auto Connection = GetNetConnection())
      {
        const float AvgRTT = Connection->AvgLag;
        const float Ping = FMath::Min(AvgRTT / 2.f, MaxExpectedPing);
        const float NewWorldTime = ServerWorldTime + Ping;
        const float SignedTimeDiscrepancy = NewWorldTime - Client_SyncedWorldTime;
        const float TimeDiscrepancy = FMath::Abs(SignedTimeDiscrepancy);
        if (TimeDiscrepancy > MaxClientTimeDifference)
        {
          // If the client time deviates more than the set limit we sync the world time with the server.
          GMC_LOG(
            VeryVerbose,
            TEXT("Synced client world time with server, time discrepancy was %f s (max allowed is %f s): ")
            TEXT("new world time = %f s | old world time = %f s | avg ping = %.0f ms | avg jitter = %.0f ms"),
            TimeDiscrepancy,
            MaxClientTimeDifference,
            NewWorldTime,
            Client_SyncedWorldTime,
            Ping * 1000.f/*convert to ms*/,
            Connection->GetAverageJitterInMS()
          )
          Client_SyncedWorldTime = NewWorldTime;

          GMC_LOG(
            Verbose,
            TEXT("Corrected client time discrepancy of %f seconds."),
            TimeDiscrepancy
          )
        }
        else
        {
          GMC_LOG(
            VeryVerbose,
            TEXT("Queried server time, client is %f seconds %s."),
            TimeDiscrepancy,
            SignedTimeDiscrepancy > 0.f ? TEXT("behind") : TEXT("ahead")
          )

          if (SignedTimeDiscrepancy < 0.f)
          {
            // The client is ahead of the server, set the flag to slow time down (processed and reset in @see Client_UpdateWorldTime).
            Client_bSlowWorldTime = true;
          }
        }
      }
      return;
    }
  }
  GMC_LOG(Warning, TEXT("Client time could not not be synced with the server."))
  checkGMC(false)
}

void AGenPlayerController::SetInterpolationDelay(float Delay) const
{
  TArray<AActor*> Actors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGenPawn::StaticClass(), Actors);
  for (const auto Actor : Actors)
  {
    if (const auto GenPawn = Cast<AGenPawn>(Actor))
    {
      if (!GenPawn->IsLocallyControlled())
      {
        if (const auto MovementComponent = Cast<UGenMovementReplicationComponent>(GenPawn->GetMovementComponent()))
        {
          MovementComponent->SimulationDelay = Delay;
        }
      }
    }
  }
}

void AGenPlayerController::SetInterpolationMethod(int32 Method) const
{
  if (!(Method >= 0 && Method < static_cast<uint8>(EInterpolationMethod::MAX)))
  {
    return;
  }

  TArray<AActor*> Actors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGenPawn::StaticClass(), Actors);
  for (const auto Actor : Actors)
  {
    if (const auto GenPawn = Cast<AGenPawn>(Actor))
    {
      if (!GenPawn->IsLocallyControlled())
      {
        if (const auto MovementComponent = Cast<UGenMovementReplicationComponent>(GenPawn->GetMovementComponent()))
        {
          MovementComponent->SetInterpolationMethod(static_cast<EInterpolationMethod>(Method));
        }
      }
    }
  }
}

void AGenPlayerController::SetExtrapolationAllowed(bool bAllowed) const
{
  TArray<AActor*> Actors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGenPawn::StaticClass(), Actors);
  for (const auto Actor : Actors)
  {
    if (const auto GenPawn = Cast<AGenPawn>(Actor))
    {
      if (!GenPawn->IsLocallyControlled())
      {
        if (const auto MovementComponent = Cast<UGenMovementReplicationComponent>(GenPawn->GetMovementComponent()))
        {
          MovementComponent->bAllowExtrapolation = bAllowed;
        }
      }
    }
  }
}

void AGenPlayerController::SetSmoothCollision(bool bSmoothLocation, bool bSmoothRotation) const
{
  TArray<AActor*> Actors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGenPawn::StaticClass(), Actors);
  for (const auto Actor : Actors)
  {
    if (const auto GenPawn = Cast<AGenPawn>(Actor))
    {
      if (!GenPawn->IsLocallyControlled())
      {
        if (const auto MovementComponent = Cast<UGenMovementReplicationComponent>(GenPawn->GetMovementComponent()))
        {
          MovementComponent->bSmoothCollisionLocation = bSmoothLocation;
          MovementComponent->bSmoothCollisionRotation = bSmoothRotation;
        }
      }
    }
  }
}
