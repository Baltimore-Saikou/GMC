// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

// Unreal Engine grid scale: 1uu = 1cm.
#define UU_METER      (100.f)
#define UU_CENTIMETER (1.f)
#define UU_MILLIMETER (1.e-1f)
#define UU_MICROMETER (1.e-4f)
#define UU_NANOMETER  (1.e-7f)

// Values of the dot product between two normalized vectors at different angles (delta degrees).
#define DOT_PRODUCT_0   (1.f)
#define DOT_PRODUCT_15  (0.966f)
#define DOT_PRODUCT_30  (0.866f)
#define DOT_PRODUCT_45  (0.707f)
#define DOT_PRODUCT_60  (0.5f)
#define DOT_PRODUCT_75  (0.259f)
#define DOT_PRODUCT_90  (0.f)
#define DOT_PRODUCT_105 (-0.259f)
#define DOT_PRODUCT_120 (-0.5f)
#define DOT_PRODUCT_135 (-0.707f)
#define DOT_PRODUCT_150 (-0.866f)
#define DOT_PRODUCT_165 (-0.966f)
#define DOT_PRODUCT_180 (-1.f)

// Get a variable name as string.
#define VAR2STR(Var) (#Var)

// Print the value of a bool expression.
#define BOOL2STR(Expr) ((Expr) ? TEXT("true") : TEXT("false"))

// Enables a nicer way to handle unused out parameters.
template<typename T> T& Unused(T&& Var) { return Var; }
#define UNUSED(Type) (Unused(Type{}))

// Returns the net role enum value as FString.
FORCEINLINE FString DebugGetNetRoleAsString(ENetRole Role)
{
  switch (Role)
  {
    case ROLE_None: return TEXT("None");
    case ROLE_SimulatedProxy: return TEXT("Simulated Proxy");
    case ROLE_AutonomousProxy: return TEXT("Autonomous Proxy");
    case ROLE_Authority: return TEXT("Authority");
    case ROLE_MAX: return TEXT("MAX");
    default: checkNoEntry();
  }
  return "";
}

#undef PAWN_REF
#undef LOG_CATEGORY

#if !NO_LOGGING

#ifdef __COUNTER__
// Print a message on the screen for a specific duration.
#define DEBUG_PRINT_MSG(Duration, FormatStr, ...)\
  if (GEngine) {\
    GEngine->AddOnScreenDebugMessage(\
      uint64(sizeof __FILE__ + sizeof __func__ + __LINE__ + __COUNTER__),\
      Duration,\
      FColor::Orange,\
      FString::Printf(TEXT(FormatStr), __VA_ARGS__),\
      false,\
      FVector2D(0.9, 0.9)\
    );\
  }
#else
#define DEBUG_PRINT_MSG(Duration, FormatStr, ...)
#endif


#ifdef GMC_REPLICATION_COMPONENT_LOG
#define PAWN_REF PawnOwner
#define LOG_CATEGORY LogGMCReplication
#endif

#ifdef GMC_MOVEMENT_COMPONENT_LOG
#define PAWN_REF PawnOwner
#define LOG_CATEGORY LogGMCMovement
#endif

#ifdef GMC_CONTROLLER_LOG
#define PAWN_REF GetPawn()
#define LOG_CATEGORY LogGMCController
#endif

#ifdef GMC_PAWN_LOG
#define PAWN_REF this
#define LOG_CATEGORY LogGMCPawn
#endif

#if defined PAWN_REF && defined LOG_CATEGORY
// Logging macro wrapper, prepends additional debug info.
#define GMC_LOG(Verbosity, Format, ...)\
  {\
    if (PAWN_REF) {\
      FString\
      gmc_Time = FDateTime::Now().GetTimeOfDay().ToString(),\
      gmc_PlayerName = PAWN_REF->GetHumanReadableName(),\
      gmc_ObjectName = PAWN_REF->GetName(),\
      gmc_NetworkRole = TEXT("");\
      if (ENetRole gmc_Role = PAWN_REF->GetLocalRole(); gmc_Role == ROLE_Authority) {\
        FString gmc_Control = PAWN_REF->IsLocallyControlled() ? TEXT("local  ") : TEXT("remote ");\
        gmc_NetworkRole = gmc_Control + DebugGetNetRoleAsString(gmc_Role);\
      }\
      else {\
        gmc_NetworkRole = DebugGetNetRoleAsString(gmc_Role);\
      }\
      LOG(LOG_CATEGORY, Verbosity, OBJECT_INFO_TEXT(Format),\
        OBJECT_INFO(gmc_Time, gmc_PlayerName, gmc_ObjectName, gmc_NetworkRole), __VA_ARGS__)\
    }\
    else {\
      LOG(LOG_CATEGORY, Error, TEXT("| GMC_LOG failed, PAWN_REF was nullptr | FORMAT-MSG: ") Format, __VA_ARGS__)\
    }\
  }
#define GMC_CLOG(Condition, Verbosity, Format, ...)\
  if (Condition) {\
    GMC_LOG(Verbosity, Format, __VA_ARGS__)\
  }
#define OBJECT_INFO_TEXT(Text) TEXT("%s %-20s (%-s: %-16s): " Text)
#define OBJECT_INFO(Time, PlayerName, ObjectName, NetworkRole) *Time, *PlayerName, *ObjectName, *NetworkRole
#define LOG(Category, Verbosity, Format, ...) UE_LOG(Category, Verbosity, Format, __VA_ARGS__)
#else
// Use the standard log if no pawn reference or logging category was set (using "LogTemp" as logging category instead).
#define GMC_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, Format, __VA_ARGS__)
#define GMC_CLOG(Condition, Verbosity, Format, ...) UE_CLOG(Condition, LogTemp, Verbosity, Format, __VA_ARGS__)
#endif

#else

#define DEBUG_PRINT_MSG(Duration, FormatStr, ...)
#define GMC_LOG(Verbosity, Format, ...)
#define GMC_CLOG(Condition, Verbosity, Format, ...)
#define OBJECT_INFO_TEXT(Text)
#define OBJECT_INFO(Time, FileName, PlayerName, ObjectName, NetworkRole)
#define LOG(Category, Verbosity, Format, ...)

#endif

#pragma warning(push)
#pragma warning(disable:4668)

// Activates additional check macros in the GMC module for more exhaustive debugging. Add
//   PrivateDefinitions.Add("GMC_DO_CHECK_EXTENSIVE");
// to the GMC.Build.cs to enable. Only works in build configurations that have "DO_CHECK" enabled as well.
#if DO_CHECK && GMC_DO_CHECK_EXTENSIVE
#define checkGMC(expr) check(expr)
#define checkfGMC(expr, format, ...) checkf(expr, format, __VA_ARGS__)
#define checkNoEntryGMC() checkNoEntry()
#define checkCodeGMC(Code) checkCode(Code)
#define verifyGMC(Code) verify(Code)
#define verifyfGMC(Code) verifyf(Code)
#else
#define checkGMC(expr)
#define checkfGMC(expr, format, ...)
#define checkNoEntryGMC()
#define checkCodeGMC(Code)
#define verifyGMC(Code)
#define verifyfGMC(Code)
#endif

#pragma warning(pop)
