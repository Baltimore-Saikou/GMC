// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

// Returns the movement mode enum value as FString.
FORCEINLINE FString DebugGetMovementModeAsString(uint8 MovementMode)
{
  switch (MovementMode)
  {
    case 0: return TEXT("None");
    case 1: return TEXT("Grounded");
    case 2: return TEXT("Airborne");
    case 3: return TEXT("Buoyant");
    case 4: return TEXT("Custom 1");
    case 5: return TEXT("Custom 2");
    case 6: return TEXT("Custom 3");
    case 7: return TEXT("Custom 4");
    case 8: return TEXT("Custom 5");
    case 9: return TEXT("Custom 6");
    case 10: return TEXT("Custom 7");
    case 11: return TEXT("Custom 8");
    case 12: return TEXT("Custom 9");
    case 13: return TEXT("Custom 10");
    case 14: return TEXT("Custom 11");
    case 15: return TEXT("Custom 12");
    case 16: return TEXT("MAX");
    default: return FString();
  }
  checkNoEntry()
}

#undef FLog
#undef CFLog

#if !NO_LOGGING

// Utility macros to quickly log messages from organic movement component functions.
#define FLog(Verbosity, Format, ...)\
  GMC_LOG(Verbosity, TEXT("UGenOrganicMovementComponent::%s: ") TEXT(Format), *FString(__func__), __VA_ARGS__)
#define CFLog(Condition, Verbosity, Format, ...)\
  GMC_CLOG(Condition, Verbosity, TEXT("UGenOrganicMovementComponent::%s: ") TEXT(Format), *FString(__func__), __VA_ARGS__)

#else

#define FLog(Verbosity, Format, ...)
#define CFLog(Condition, Verbosity, Format, ...)

#endif

#if ALLOW_CONSOLE && !NO_LOGGING

// Stat and/or log any values relating to the movement physics of the pawn.
#define DEBUG_STAT_AND_LOG_ORGANIC_MOVEMENT_VALUES\
  if (GMCCVars::StatOrganicMovementValues != 0 || GMCCVars::LogOrganicMovementValues != 0) {\
    const FVector Location = UpdatedComponent->GetComponentLocation();\
    const FVector FrameLocationDelta = Location - GetStartLocation();\
    if (GMCCVars::StatOrganicMovementValues != 0) {\
      DEBUG_PRINT_MSG(0, "Force : %s", *GetTransientForce().ToString())\
      DEBUG_PRINT_MSG(0, "Acceleration : %s", *GetTransientAcceleration().ToString())\
      DEBUG_PRINT_MSG(0, "Velocity : %s", *GetVelocity().ToString())\
      DEBUG_PRINT_MSG(0, "Location : %s", *Location.ToString())\
      DEBUG_PRINT_MSG(0, "SpeedZ : %f", FVector(0.f, 0.f, GetVelocity().Z).Size())\
      DEBUG_PRINT_MSG(0, "SpeedXY : %f", FVector(GetVelocity().X, GetVelocity().Y, 0.f).Size())\
      DEBUG_PRINT_MSG(0, "SpeedTotal : %f", GetVelocity().Size())\
      DEBUG_PRINT_MSG(0, "DeltaTime : %f", GetMoveDeltaTime())\
      DEBUG_PRINT_MSG(0, "MovementMode : %s", *DebugGetMovementModeAsString(MovementMode))\
    }\
    if (GMCCVars::LogOrganicMovementValues != 0) {\
      GMC_LOG(Log, TEXT("MovementMode         : %s"), *DebugGetMovementModeAsString(MovementMode))\
      GMC_LOG(Log, TEXT("DeltaTime            : %f"), GetMoveDeltaTime())\
      GMC_LOG(Log, TEXT("Speed Total          : %f"), GetVelocity().Size())\
      GMC_LOG(Log, TEXT("Speed XY             : %f"), FVector(GetVelocity().X, GetVelocity().Y, 0.f).Size())\
      GMC_LOG(Log, TEXT("Speed Z              : %f"), FVector(0.f, 0.f, GetVelocity().Z).Size())\
      GMC_LOG(Log, TEXT("Location             :           % 15.6f, % 15.6f, % 15.6f"), Location.X, Location.Y, Location.Z, (Location - GetStartLocation()).Size())\
      GMC_LOG(Log, TEXT("Velocity             :           % 15.6f, % 15.6f, % 15.6f"), GetVelocity().X, GetVelocity().Y, GetVelocity().Z, (GetVelocity() - GetStartVelocity()).Size())\
      GMC_LOG(Log, TEXT("Acceleration         :           % 15.6f, % 15.6f, % 15.6f"), GetTransientAcceleration().X, GetTransientAcceleration().Y, GetTransientAcceleration().Z)\
      GMC_LOG(Log, TEXT("Force                :           % 15.6f, % 15.6f, % 15.6f"), GetTransientForce().X, GetTransientForce().Y, GetTransientForce().Z)\
    }\
  }

#define DEBUG_LOG_AUTO_RESOLVE_PENETRATION_START\
  const FVector DebugStartPosition = UpdatedComponent->GetComponentLocation();

#define DEBUG_LOG_AUTO_RESOLVE_PENETRATION_END\
  const FVector DebugResolvedPositionDifference = UpdatedComponent->GetComponentLocation() - DebugStartPosition;\
  const float DebugResolvedPositionDifferenceSize = DebugResolvedPositionDifference.Size();\
  if (DebugResolvedPositionDifferenceSize > KINDA_SMALL_NUMBER)\
  {\
    FLog(\
      Verbose,\
      "Adjusted pawn position by %s (distance = %f).",\
      *DebugResolvedPositionDifference.ToString(),\
      DebugResolvedPositionDifferenceSize\
    )\
  }

#define DEBUG_LOG_NAN_DIAGNOSTIC\
  GMC_CLOG(GetVelocity().ContainsNaN(), Warning, TEXT("Velocity (%s) contains NAN."), *GetVelocity().ToString())\
  GMC_CLOG(GetTransientAcceleration().ContainsNaN(), Warning, TEXT("Acceleration (%s) contains NAN."), *GetTransientAcceleration().ToString())\
  GMC_CLOG(GetTransientForce().ContainsNaN(), Warning, TEXT("Force (%s) contains NAN."), *GetTransientForce().ToString())

#else

#define DEBUG_STAT_AND_LOG_ORGANIC_MOVEMENT_VALUES
#define DEBUG_LOG_AUTO_RESOLVE_PENETRATION_START
#define DEBUG_LOG_AUTO_RESOLVE_PENETRATION_END
#define DEBUG_LOG_NAN_DIAGNOSTIC

#endif
