// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#if ALLOW_CONSOLE && !NO_LOGGING

// Display the current ping to the server on a client machine.
#define DEBUG_STAT_PING\
    if (GMCCVars::StatPing != 0) {\
      if (GetLocalRole() == ROLE_AutonomousProxy) {\
        if (const auto World = GetWorld()) {\
          if (const auto GameState = World->GetGameState()) {\
            if (const UNetConnection* Connection = GetNetConnection()) {\
              const float Ping = (Connection->AvgLag / 2.f) * 1000.f;\
              DEBUG_PRINT_MSG(0, "ping to server: %.0f ms", Ping)\
              DEBUG_PRINT_MSG(0, "jitter: %.0f ms", Connection->GetAverageJitterInMS())\
            }\
          }\
        }\
      }\
    }

// Log the current world time on the server and the synced world time on the client with a UTC timestamp for comparison.
#define DEBUG_LOG_NET_WORLD_TIME\
    if (GMCCVars::LogNetWorldTime != 0) {\
      if (GetLocalRole() == ROLE_Authority && IsLocalController()) {\
        if (const UWorld* World = GetWorld()) {\
          if (const AGameStateBase* GameState = World->GetGameState()) {\
            GMC_LOG(Log, TEXT("UtcNow: %s | ServerWorldTime = %12.6f s"),\
              *FDateTime::UtcNow().GetTimeOfDay().ToString(), GameState->GetServerWorldTimeSeconds())\
          }\
        }\
      }\
      if (GetLocalRole() == ROLE_AutonomousProxy) {\
        if (const UWorld* World = GetWorld()) {\
          if (const AGameStateBase* GameState = World->GetGameState()) {\
            const float ServerWorldTime = GameState->GetServerWorldTimeSeconds();\
            float AvgRTT = NAN;\
            if (const UNetConnection* Connection = GetNetConnection()) {\
              AvgRTT = Connection->AvgLag;\
            }\
            GMC_LOG(Log, TEXT("UtcNow: %s | ClientWorldTime = %12.6f s | Ping = %4.0f ms"),\
              *FDateTime::UtcNow().GetTimeOfDay().ToString(), Client_SyncedWorldTime, AvgRTT / 2.f * 1000.f)\
          }\
        }\
      }\
    }

#else

  #define DEBUG_STAT_PING
  #define DEBUG_LOG_NET_WORLD_TIME

#endif
