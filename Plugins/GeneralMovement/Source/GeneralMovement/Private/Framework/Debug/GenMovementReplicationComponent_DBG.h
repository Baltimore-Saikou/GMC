// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#if ALLOW_CONSOLE && !NO_LOGGING

// Print movement values for pawns with the specified net role on the screen.
#define DEBUG_PRINT_LOCAL_STATS(PawnNetRole)\
  if (PawnOwner->GetLocalRole() == PawnNetRole) {\
    const AController* Controller = PawnOwner->GetController();\
    const FRotator ControlRotation = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;\
    const FVector InputVector = GetLastInputVector().GetClampedToMaxSize(1.f);\
    DEBUG_PRINT_MSG(0, "");\
    DEBUG_PRINT_MSG(0, "Z: %f", ControlRotation.Yaw);\
    DEBUG_PRINT_MSG(0, "Y: %f", ControlRotation.Pitch);\
    DEBUG_PRINT_MSG(0, "X: %f", ControlRotation.Roll);\
    DEBUG_PRINT_MSG(0, "ControlRotation:");\
    DEBUG_PRINT_MSG(0, "Z: %f", PawnOwner->GetActorRotation().Yaw);\
    DEBUG_PRINT_MSG(0, "Y: %f", PawnOwner->GetActorRotation().Pitch);\
    DEBUG_PRINT_MSG(0, "X: %f", PawnOwner->GetActorRotation().Roll);\
    DEBUG_PRINT_MSG(0, "Rotation:");\
    DEBUG_PRINT_MSG(0, "Z: %f", PawnOwner->GetActorLocation().Z);\
    DEBUG_PRINT_MSG(0, "Y: %f", PawnOwner->GetActorLocation().Y);\
    DEBUG_PRINT_MSG(0, "X: %f", PawnOwner->GetActorLocation().X);\
    DEBUG_PRINT_MSG(0, "Location:");\
    DEBUG_PRINT_MSG(0, "Z: %f", Velocity.Z);\
    DEBUG_PRINT_MSG(0, "Y: %f", Velocity.Y);\
    DEBUG_PRINT_MSG(0, "X: %f", Velocity.X);\
    DEBUG_PRINT_MSG(0, "Velocity:");\
    DEBUG_PRINT_MSG(0, "Z: %f", InputVector.Z);\
    DEBUG_PRINT_MSG(0, "Y: %f", InputVector.Y);\
    DEBUG_PRINT_MSG(0, "X: %f", InputVector.X);\
    DEBUG_PRINT_MSG(0, "ControlInput:");\
    DEBUG_PRINT_MSG(0, "OwnerNetRole: %s", *DebugGetNetRoleAsString(PawnOwner->GetLocalRole()));\
  }
#define DEBUG_DISPLAY_LOCAL_STATS\
  if (GMCCVars::StatNetMovementValues == 1) {\
    DEBUG_PRINT_LOCAL_STATS(ROLE_Authority);\
  }\
  else if (GMCCVars::StatNetMovementValues == 2) {\
    DEBUG_PRINT_LOCAL_STATS(ROLE_AutonomousProxy);\
  }\
  else if (GMCCVars::StatNetMovementValues == 3) {\
    DEBUG_PRINT_LOCAL_STATS(ROLE_SimulatedProxy);\
  }

// Print replication related values for pawns with the specified net role on the screen.
#define DEBUG_PRINT_NET_STATS(PawnNetRole)\
  if (PawnOwner->GetLocalRole() == PawnNetRole) {\
    DEBUG_PRINT_MSG(0, "");\
    DEBUG_PRINT_MSG(0, "bUsingExtrapolatedData: %s", bUsingExtrapolatedData ? TEXT("true") : TEXT("false"));\
    DEBUG_PRINT_MSG(0, "StateQueue.Size: %d", StateQueue.Num());\
    DEBUG_PRINT_MSG(0, "PendingMoves.Size: %d", Client_PendingMoves.Num());\
    DEBUG_PRINT_MSG(0, "MoveQueue.Size: %d", Client_MoveQueue.Num());\
    DEBUG_PRINT_MSG(0, "WorldTimeSeconds: %f", GetTime());\
    DEBUG_PRINT_MSG(0, "OwnerNetRole: %s", *DebugGetNetRoleAsString(PawnOwner->GetLocalRole()));\
  }
#define DEBUG_DISPLAY_NET_STATS\
  if (GMCCVars::StatNetContextValues == 1) {\
    DEBUG_PRINT_NET_STATS(ROLE_Authority);\
  }\
  else if (GMCCVars::StatNetContextValues == 2) {\
    DEBUG_PRINT_NET_STATS(ROLE_AutonomousProxy);\
  }\
  else if (GMCCVars::StatNetContextValues == 3) {\
    DEBUG_PRINT_NET_STATS(ROLE_SimulatedProxy);\
  }

// Display the net role of the pawn above them as floating text.
#define DEBUG_SHOW_PAWN_NET_ROLE\
  if (GMCCVars::ShowNetRole != 0) {\
    DrawDebugString(GetWorld(), FVector(0, 0, 150),\
      TEXT("Local Role: ") + DebugGetNetRoleAsString(PawnOwner->GetLocalRole()), PawnOwner, FColor::Red, 0);\
    DrawDebugString(GetWorld(), FVector(0, 0, 125),\
      TEXT("Remote Role: ") + DebugGetNetRoleAsString(PawnOwner->GetRemoteRole()), PawnOwner, FColor::Red, 0);\
  }

// Display net corrections of the client location on the client. The original position is displayed in red, the corrected (replayed)
// position in green. Unless "bAlwaysReplay" is true, this will also visualize when another state value is erroneous (e.g. rotation/control
// rotation) but the shapes will be displayed in the same location (you can check the verbose log for more details when this happens). If
// "bAlwaysReplay" is set to true, the visualization will only be triggered if there really is a significant difference between the original
// and the replayed client location (otherwise shapes would be drawn continuously).
#define DEBUG_NET_CORRECTION_ORIGINAL_CLIENT_LOCATION\
  const FVector Debug_OriginalPawnLocation = PawnOwner->GetActorLocation();
#define DEBUG_NET_CORRECTION_UPDATED_CLIENT_LOCATION\
  const FVector Debug_UpdatedPawnLocation = PawnOwner->GetActorLocation();
#define DEBUG_NET_CORRECTION_REPLAYED_CLIENT_LOCATION\
  const FVector Debug_ReplayedPawnLocation = PawnOwner->GetActorLocation();
#define DEBUG_NET_CORRECTION_DRAW_CLIENT_SHAPES\
  if (GMCCVars::ShowClientLocationCorrections != 0) {\
    if (!bAlwaysReplay || (bAlwaysReplay && !Debug_OriginalPawnLocation.Equals(Debug_ReplayedPawnLocation, 0.1f))) {\
      if (const auto World = GetWorld()) {\
        GMC_LOG(Log,\
          TEXT("Client was corrected by server: original location = % 10.3f, % 10.3f, % 10.3f | updated location = % 10.3f, % 10.3f, % 10.3f | corrected location = % 10.3f, % 10.3f, % 10.3f"),\
          Debug_OriginalPawnLocation.X, Debug_OriginalPawnLocation.Y, Debug_OriginalPawnLocation.Z,\
          Debug_UpdatedPawnLocation.X, Debug_UpdatedPawnLocation.Y, Debug_UpdatedPawnLocation.Z,\
          Debug_ReplayedPawnLocation.X, Debug_ReplayedPawnLocation.Y, Debug_ReplayedPawnLocation.Z)\
        constexpr float DebugShapeLifetime = 5.f;\
        constexpr float DebugShapeThickness = 1.f;\
        const UShapeComponent* RootCollision = Cast<UShapeComponent>(GenPawnOwner->GetRootComponent());\
        if (const UBoxComponent* RootBox = Cast<UBoxComponent>(RootCollision)) {\
          const FVector BoxScaledExtent = RootBox->GetScaledBoxExtent();\
          const FQuat BoxRotation = RootBox->GetComponentQuat();\
          DrawDebugBox(\
            World,\
            Debug_OriginalPawnLocation,\
            BoxScaledExtent,\
            BoxRotation,\
            FColor::Red,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
          DrawDebugBox(\
            World,\
            Debug_ReplayedPawnLocation,\
            BoxScaledExtent,\
            BoxRotation,\
            FColor::Green,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
        }\
        else if (const USphereComponent* RootSphere = Cast<USphereComponent>(RootCollision)) {\
          constexpr int32 SphereSegments = 32;\
          const float SphereScaledRadius = RootSphere->GetScaledSphereRadius();\
          DrawDebugSphere(\
            World,\
            Debug_OriginalPawnLocation,\
            SphereScaledRadius,\
            SphereSegments,\
            FColor::Red,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
          DrawDebugSphere(\
            World,\
            Debug_ReplayedPawnLocation,\
            SphereScaledRadius,\
            SphereSegments,\
            FColor::Green,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
        }\
        else if (const UFlatCapsuleComponent* RootHCapsule = Cast<UFlatCapsuleComponent>(RootCollision)) {\
          const float FlatCapsuleScaledHalfHeight = RootHCapsule->GetScaledCapsuleHalfHeight();\
          const float FlatCapsuleScaledRadius = RootHCapsule->GetScaledCapsuleRadius();\
          FQuat FlatCapsuleRotation = (UpdatedComponent->GetComponentQuat() * FQuat(RootHCapsule->GetGenCapsuleRotation())).GetNormalized();\
          DrawDebugCapsule(\
            World,\
            Debug_OriginalPawnLocation,\
            FlatCapsuleScaledHalfHeight,\
            FlatCapsuleScaledRadius,\
            FlatCapsuleRotation,\
            FColor::Red,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
          DrawDebugCapsule(\
            World,\
            Debug_ReplayedPawnLocation,\
            FlatCapsuleScaledHalfHeight,\
            FlatCapsuleScaledRadius,\
            FlatCapsuleRotation,\
            FColor::Green,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
        }\
        else if (const UCapsuleComponent* RootVCapsule = Cast<UCapsuleComponent>(RootCollision)) {\
          const float CapsuleScaledHalfHeight = RootVCapsule->GetScaledCapsuleHalfHeight();\
          const float CapsuleScaledRadius = RootVCapsule->GetScaledCapsuleRadius();\
          const FQuat CapsuleRotation = RootVCapsule->GetComponentQuat();\
          DrawDebugCapsule(\
            World,\
            Debug_OriginalPawnLocation,\
            CapsuleScaledHalfHeight,\
            CapsuleScaledRadius,\
            CapsuleRotation,\
            FColor::Red,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
          DrawDebugCapsule(\
            World,\
            Debug_ReplayedPawnLocation,\
            CapsuleScaledHalfHeight,\
            CapsuleScaledRadius,\
            CapsuleRotation,\
            FColor::Green,\
            false,\
            DebugShapeLifetime,\
            (uint8)'\000',\
            DebugShapeThickness\
          );\
        }\
      }\
    }\
  }

// Visualize client location errors on the server. The erroneous client location is displayed in red, the server location in green. This
// will also visualize when another state value is erroneous (e.g. rotation/control rotation) but the shapes will be displayed in the same
// location in that case (you can check the verbose log for more details when this happens).
#define DEBUG_SHOW_CLIENT_LOCATION_ERRORS_ON_SERVER\
  if (GMCCVars::ShowClientLocationErrors != 0) {\
    const auto World = GetWorld();\
    if (World && !Server_bLastClientMoveWasValid) {\
      constexpr float DebugShapeLifetime = 5.f;\
      constexpr float DebugShapeThickness = 1.f;\
      const UShapeComponent* RootCollision = Cast<UShapeComponent>(GenPawnOwner->GetRootComponent());\
      if (const UBoxComponent* RootBox = Cast<UBoxComponent>(RootCollision)) {\
        const FVector BoxScaledExtent = RootBox->GetScaledBoxExtent();\
        const FQuat BoxRotation = RootBox->GetComponentQuat();\
        DrawDebugBox(\
          World,\
          ClientMove.OutLocation,\
          BoxScaledExtent,\
          BoxRotation,\
          FColor::Red,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness\
        );\
        DrawDebugBox(\
          World,\
          ClientMove.OutLocation,\
          BoxScaledExtent,\
          BoxRotation,\
          FColor::Green,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness\
        );\
      }\
      else if (const USphereComponent* RootSphere = Cast<USphereComponent>(RootCollision)) {\
        constexpr int32 SphereSegments = 32;\
        const float SphereScaledRadius = RootSphere->GetScaledSphereRadius();\
        DrawDebugSphere(\
          World,\
          ClientMove.OutLocation,\
          SphereScaledRadius,\
          SphereSegments,\
          FColor::Red,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness);\
        DrawDebugSphere(\
          World,\
          ClientMove.OutLocation,\
          SphereScaledRadius,\
          SphereSegments,\
          FColor::Green,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness);\
      }\
      else if (const UFlatCapsuleComponent* RootHCapsule = Cast<UFlatCapsuleComponent>(RootCollision)) {\
        const float FlatCapsuleScaledHalfHeight = RootHCapsule->GetScaledCapsuleHalfHeight();\
        const float FlatCapsuleScaledRadius = RootHCapsule->GetScaledCapsuleRadius();\
        FQuat FlatCapsuleRotation = (UpdatedComponent->GetComponentQuat() * FQuat(RootHCapsule->GetGenCapsuleRotation())).GetNormalized();\
        DrawDebugCapsule(\
          World,\
          ClientMove.OutLocation,\
          FlatCapsuleScaledHalfHeight,\
          FlatCapsuleScaledRadius,\
          FlatCapsuleRotation,\
          FColor::Red,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness\
        );\
        DrawDebugCapsule(\
          World,\
          ClientMove.OutLocation,\
          FlatCapsuleScaledHalfHeight,\
          FlatCapsuleScaledRadius,\
          FlatCapsuleRotation,\
          FColor::Green,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness\
        );\
      }\
      else if (const UCapsuleComponent* RootVCapsule = Cast<UCapsuleComponent>(RootCollision)) {\
        const float CapsuleScaledHalfHeight = RootVCapsule->GetScaledCapsuleHalfHeight();\
        const float CapsuleScaledRadius = RootVCapsule->GetScaledCapsuleRadius();\
        const FQuat CapsuleRotation = RootVCapsule->GetComponentQuat();\
        DrawDebugCapsule(\
          World,\
          ClientMove.OutLocation,\
          CapsuleScaledHalfHeight,\
          CapsuleScaledRadius,\
          CapsuleRotation,\
          FColor::Red,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness\
        );\
        DrawDebugCapsule(\
          World,\
          ClientMove.OutLocation,\
          CapsuleScaledHalfHeight,\
          CapsuleScaledRadius,\
          CapsuleRotation,\
          FColor::Green,\
          false,\
          DebugShapeLifetime,\
          (uint8)'\000',\
          DebugShapeThickness\
        );\
      }\
    }\
  }

// Log the execution of moves (excluding client replays).
#define DEBUG_LOG_MOVE_BEFORE_ITERATION\
  if (GMCCVars::LogMoveExecution != 0 && !Client_bIsReplaying) {\
    const FVector PawnOwnerLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnOwnerRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
      TEXT("MoveExecutionLog: Iteration %2d/%2d --> IN  : Timestamp = %12.6f | DeltaTime = %8.6f | RemainingTime = %8.6f | SubDeltaTime = %8.6f | InputVector = % 9.6f, % 9.6f, % 9.6f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) | InVelocity  = % 15.6f, % 15.6f, % 15.6f | InLocation  = % 15.6f, % 15.6f, % 15.6f | InRotation  = % 11.6f, % 11.6f, % 11.6f | InControlRotation  = % 11.6f, % 11.6f, % 11.6f |                                       %*s |"),\
      Iterations,\
      MaxIterations,\
      Move.Timestamp,\
      Move.DeltaTime,\
      RemainingTime,\
      SubDeltaTime,\
      Move.InputVector.X, Move.InputVector.Y, Move.InputVector.Z,\
      Move.bInputFlag1, Move.bInputFlag2, Move.bInputFlag3, Move.bInputFlag4, Move.bInputFlag5, Move.bInputFlag6, Move.bInputFlag7, Move.bInputFlag8,\
      Move.bInputFlag9, Move.bInputFlag10, Move.bInputFlag11, Move.bInputFlag12, Move.bInputFlag13, Move.bInputFlag14, Move.bInputFlag15, Move.bInputFlag16,\
      IterationVelocity.X, IterationVelocity.Y, IterationVelocity.Z,\
      PawnOwnerLocation.X, PawnOwnerLocation.Y, PawnOwnerLocation.Z,\
      PawnOwnerRotation.Roll, PawnOwnerRotation.Pitch, PawnOwnerRotation.Yaw,\
      PawnOwnerControlRotation.Roll, PawnOwnerControlRotation.Yaw, PawnOwnerControlRotation.Yaw,\
      27, TEXT(" "))\
  }
#define DEBUG_LOG_MOVE_AFTER_ITERATION\
  if (GMCCVars::LogMoveExecution != 0 && !Client_bIsReplaying) {\
    const FVector PawnLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
      TEXT("MoveExecutionLog: Iteration %2d/%2d <-- OUT : Timestamp = %12.6f |                                                                                                                                                                                 %*s | OutVelocity = % 15.6f, % 15.6f, % 15.6f | OutLocation = % 15.6f, % 15.6f, % 15.6f | OutRotation = % 11.6f, % 11.6f, % 11.6f | OutControlRotation = % 11.6f, % 11.6f, % 11.6f | LocationDelta = % 15.6f, % 15.6f, % 15.6f |"),\
      Iterations,\
      MaxIterations,\
      Move.Timestamp,\
      5, TEXT(" "),\
      IterationVelocity.X, IterationVelocity.Y, IterationVelocity.Z,\
      PawnLocation.X, PawnLocation.Y, PawnLocation.Z,\
      PawnRotation.Roll, PawnRotation.Pitch, PawnRotation.Yaw,\
      PawnOwnerControlRotation.Roll, PawnOwnerControlRotation.Yaw, PawnOwnerControlRotation.Yaw,\
      LocationDelta.X, LocationDelta.Y, LocationDelta.Z)\
  }

// Log client replay.
#define DEBUG_LOG_MOVE_QUEUE_SIZE_BEFORE_CLEARING\
  if (GMCCVars::LogClientReplay != 0) {\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Clearing Moves ---> Queue Size Before Clearing  = %3d"),\
    Client_MoveQueue.Num())\
  }
#define DEBUG_LOG_MOVE_QUEUE_SIZE_AFTER_CLEARING\
  if (GMCCVars::LogClientReplay != 0) {\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Clearing Moves ---> Queue Size  After Clearing  = %3d"),\
    Client_MoveQueue.Num())\
  }
#define DEBUG_LOG_REPLAY_CLIENT_STATE_INITIAL\
  if (GMCCVars::LogClientReplay != 0) {\
    const FVector PawnOwnerVelocity = GetVelocity();\
    const FVector PawnOwnerLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnOwnerRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: --- Replay Start ---"))\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Initial Client State               :                                                                              %*s |         Velocity = % 12.3f, % 12.3f, % 12.3f |         Location = % 12.3f, % 12.3f, % 12.3f |         Actor Rotation = % 12.3f, % 12.3f, % 12.3f |         Control Rotation = % 12.3f, % 12.3f, % 12.3f"),\
    10, TEXT(" "),\
    PawnOwnerVelocity.X,\
    PawnOwnerVelocity.Y,\
    PawnOwnerVelocity.Z,\
    PawnOwnerLocation.X,\
    PawnOwnerLocation.Y,\
    PawnOwnerLocation.Z,\
    PawnOwnerRotation.Roll,\
    PawnOwnerRotation.Pitch,\
    PawnOwnerRotation.Yaw,\
    PawnOwnerControlRotation.Roll,\
    PawnOwnerControlRotation.Pitch,\
    PawnOwnerControlRotation.Yaw)\
  }
#define DEBUG_LOG_REPLAY_SERVER_STATE\
  if (GMCCVars::LogClientReplay != 0) {\
    const FVector ServerStateVelocity = ServerState_AutonomousProxy().Velocity;\
    const FVector ServerStateLocation = ServerState_AutonomousProxy().Location;\
    const FRotator ServerStateRotation = ServerState_AutonomousProxy().Rotation;\
    const FRotator ServerStateControlRotation = ServerState_AutonomousProxy().ControlRotation;\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Received Server State              : .Timestamp = %10.3f |                                                        %*s |        .Velocity = % 12.3f, % 12.3f, % 12.3f |        .Location = % 12.3f, % 12.3f, % 12.3f |              .Rotation = % 12.3f, % 12.3f, % 12.3f |         .ControlRotation = % 12.3f, % 12.3f, % 12.3f"),\
    ServerState_AutonomousProxy().Timestamp,\
    6, TEXT(" "),\
    ServerStateVelocity.X,\
    ServerStateVelocity.Y,\
    ServerStateVelocity.Z,\
    ServerStateLocation.X,\
    ServerStateLocation.Y,\
    ServerStateLocation.Z,\
    ServerStateRotation.Roll,\
    ServerStateRotation.Pitch,\
    ServerStateRotation.Yaw,\
    ServerStateControlRotation.Roll,\
    ServerStateControlRotation.Pitch,\
    ServerStateControlRotation.Yaw)\
  }
#define DEBUG_LOG_REPLAY_CLIENT_STATE_BEFORE_REPLAY\
  if (GMCCVars::LogClientReplay != 0) {\
    const FVector PawnOwnerVelocity = GetVelocity();\
    const FVector PawnOwnerLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnOwnerRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Updated Client State Before Replay :                                                                              %*s |         Velocity = % 12.3f, % 12.3f, % 12.3f |         Location = % 12.3f, % 12.3f, % 12.3f |         Actor Rotation = % 12.3f, % 12.3f, % 12.3f |         Control Rotation = % 12.3f, % 12.3f, % 12.3f"),\
    10, TEXT(" "),\
    PawnOwnerVelocity.X,\
    PawnOwnerVelocity.Y,\
    PawnOwnerVelocity.Z,\
    PawnOwnerLocation.X,\
    PawnOwnerLocation.Y,\
    PawnOwnerLocation.Z,\
    PawnOwnerRotation.Roll,\
    PawnOwnerRotation.Pitch,\
    PawnOwnerRotation.Yaw,\
    PawnOwnerControlRotation.Roll,\
    PawnOwnerControlRotation.Pitch,\
    PawnOwnerControlRotation.Yaw)\
  }\
  int32 DebugMoveQueueIndex = 0;
#define DEBUG_LOG_REPLAY_CLIENT_REPLAY_LOOP\
  if (GMCCVars::LogClientReplay != 0) {\
    const FVector MoveInputVector = Move.InputVector;\
    const FVector PawnOwnerVelocity = GetVelocity();\
    const FVector PawnOwnerLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnOwnerRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Replaying ---> MoveQueue[%3d]      : .Timestamp = %10.3f | .DeltaTime = %8.6f | .InputVector = % 5.3f, % 5.3f, % 5.3f | Updated Velocity = % 12.3f, % 12.3f, % 12.3f | Updated Location = % 12.3f, % 12.3f, % 12.3f | Updated Actor Rotation = % 12.3f, % 12.3f, % 12.3f | Updated Control Rotation = % 12.3f, % 12.3f, % 12.3f"),\
    DebugMoveQueueIndex,\
    Move.Timestamp,\
    Move.DeltaTime,\
    MoveInputVector.X,\
    MoveInputVector.Y,\
    MoveInputVector.Z,\
    PawnOwnerVelocity.X,\
    PawnOwnerVelocity.Y,\
    PawnOwnerVelocity.Z,\
    PawnOwnerLocation.X,\
    PawnOwnerLocation.Y,\
    PawnOwnerLocation.Z,\
    PawnOwnerRotation.Roll,\
    PawnOwnerRotation.Pitch,\
    PawnOwnerRotation.Yaw,\
    PawnOwnerControlRotation.Roll,\
    PawnOwnerControlRotation.Pitch,\
    PawnOwnerControlRotation.Yaw)\
  }\
  ++DebugMoveQueueIndex;
#define DEBUG_LOG_REPLAY_CLIENT_STATE_AFTER_REPLAY\
  if (GMCCVars::LogClientReplay != 0) {\
    const FVector PawnOwnerVelocity = GetVelocity();\
    const FVector PawnOwnerLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnOwnerRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: Updated Client State After Replay  :                                                                              %*s |         Velocity = % 12.3f, % 12.3f, % 12.3f |         Location = % 12.3f, % 12.3f, % 12.3f |         Actor Rotation = % 12.3f, % 12.3f, % 12.3f |         Control Rotation = % 12.3f, % 12.3f, % 12.3f"),\
    10, TEXT(" "),\
    PawnOwnerVelocity.X,\
    PawnOwnerVelocity.Y,\
    PawnOwnerVelocity.Z,\
    PawnOwnerLocation.X,\
    PawnOwnerLocation.Y,\
    PawnOwnerLocation.Z,\
    PawnOwnerRotation.Roll,\
    PawnOwnerRotation.Pitch,\
    PawnOwnerRotation.Yaw,\
    PawnOwnerControlRotation.Roll,\
    PawnOwnerControlRotation.Pitch,\
    PawnOwnerControlRotation.Yaw)\
    GMC_LOG(Log,\
    TEXT("ClientReplayLog: --- Replay End ---"))\
  }

// Log the smoothing of pawns.
#define DEBUG_LOG_SMOOTHING_INTERPOLATION_DATA\
  if (GMCCVars::LogSmoothing != 0) {\
    const FVector PawnOwnerVelocity = GetVelocity();\
    const FVector PawnOwnerLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnOwnerRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnOwnerControlRotation = PawnOwner->GetControlRotation();\
    const FVector StartStateVelocity = StartState.Velocity;\
    const FVector StartStateLocation = StartState.Location;\
    const FRotator StartStateRotation = StartState.Rotation;\
    const FRotator StartStateControlRotation = StartState.ControlRotation;\
    const FVector TargetStateVelocity = TargetState.Velocity;\
    const FVector TargetStateLocation = TargetState.Location;\
    const FRotator TargetStateRotation = TargetState.Rotation;\
    const FRotator TargetStateControlRotation = TargetState.ControlRotation;\
    const FVector SmoothStateVelocity = SmoothState.Velocity;\
    const FVector SmoothStateLocation = SmoothState.Location;\
    const FRotator SmoothStateRotation = SmoothState.Rotation;\
    const FRotator SmoothStateControlRotation = SmoothState.ControlRotation;\
    const FString Method = bUsingExtrapolatedData ? TEXT("Extrapolating") : TEXT("Interpolating");\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: --- Interpolation BEGIN ---"))\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: %s ---> InterpolationTime  = % 12.3f"),\
    *Method,\
    InterpolationTime)\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: %s ---> InterpolationRatio = % 12.3f = (%9.3f - %9.3f) / (%9.3f - %9.3f) = (InterpolationTime - StartStateTimestamp) / (TargetStateTimestamp - StartStateTimestamp)"),\
    *Method,\
    InterpolationRatio,\
    InterpolationTime,\
    StartState.Timestamp,\
    TargetState.Timestamp,\
    StartState.Timestamp)\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: %s --->      StartVelocity = % 12.3f, % 12.3f, % 12.3f | StartLocation  = % 12.3f, % 12.3f, % 12.3f | StartRotation  = % 12.3f, % 12.3f, % 12.3f | StartControlRotation  = % 12.3f, % 12.3f, % 12.3f"),\
    *Method,\
    StartStateVelocity.X,\
    StartStateVelocity.Y,\
    StartStateVelocity.Z,\
    StartStateLocation.X,\
    StartStateLocation.Y,\
    StartStateLocation.Z,\
    StartStateRotation.Roll,\
    StartStateRotation.Pitch,\
    StartStateRotation.Yaw,\
    StartStateControlRotation.Roll,\
    StartStateControlRotation.Pitch,\
    StartStateControlRotation.Yaw)\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: %s --->     TargetVelocity = % 12.3f, % 12.3f, % 12.3f | TargetLocation = % 12.3f, % 12.3f, % 12.3f | TargetRotation = % 12.3f, % 12.3f, % 12.3f | TargetControlRotation = % 12.3f, % 12.3f, % 12.3f"),\
    *Method,\
    TargetStateVelocity.X,\
    TargetStateVelocity.Y,\
    TargetStateVelocity.Z,\
    TargetStateLocation.X,\
    TargetStateLocation.Y,\
    TargetStateLocation.Z,\
    TargetStateRotation.Roll,\
    TargetStateRotation.Pitch,\
    TargetStateRotation.Yaw,\
    TargetStateControlRotation.Roll,\
    TargetStateControlRotation.Pitch,\
    TargetStateControlRotation.Yaw)\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: %s --->     ResultVelocity = % 12.3f, % 12.3f, % 12.3f | ResultLocation = % 12.3f, % 12.3f, % 12.3f | ResultRotation = % 12.3f, % 12.3f, % 12.3f | ResultControlRotation = % 12.3f, % 12.3f, % 12.3f"),\
    *Method,\
    SmoothStateVelocity.X,\
    SmoothStateVelocity.Y,\
    SmoothStateVelocity.Z,\
    SmoothStateLocation.X,\
    SmoothStateLocation.Y,\
    SmoothStateLocation.Z,\
    SmoothStateRotation.Roll,\
    SmoothStateRotation.Pitch,\
    SmoothStateRotation.Yaw,\
    SmoothStateControlRotation.Roll,\
    SmoothStateControlRotation.Pitch,\
    SmoothStateControlRotation.Yaw)\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: --- Interpolation END ---"))\
  }

// Log the contents of the state queue.
#define DEBUG_LOG_STATE_QUEUE_DATA\
  if (GMCCVars::LogStateQueueData == 1 && PawnOwner->GetLocalRole() == ROLE_Authority\
    || GMCCVars::LogStateQueueData >= 2 && PawnOwner->GetLocalRole() == ROLE_SimulatedProxy) {\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: --- State Queue Contents BEGIN ---"))\
    for (int32 Index = 0; Index < StateQueue.Num(); Index++) {\
      const FState& StateQueueIndex = AccessStateQueue(Index);\
      const FVector StateQueueIndexVelocity = StateQueueIndex.Velocity;\
      const FVector StateQueueIndexLocation = StateQueueIndex.Location;\
      const FRotator StateQueueIndexRotation = StateQueueIndex.Rotation;\
      const FRotator StateQueueIndexControlRotation = StateQueueIndex.ControlRotation;\
      GMC_LOG(Log,\
        TEXT("SmoothingLog: StateQueue[%3d]. Timestamp = %12.6f | Velocity = % 12.3f, % 12.3f, % 12.3f | Location = % 12.3f, % 12.3f, % 12.3f | Rotation = % 12.3f, % 12.3f, % 12.3f | ControlRotation = % 12.3f, % 12.3f, % 12.3f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"),\
        Index,\
        StateQueueIndex.Timestamp,\
        StateQueueIndexVelocity.X,\
        StateQueueIndexVelocity.Y,\
        StateQueueIndexVelocity.Z,\
        StateQueueIndexLocation.X,\
        StateQueueIndexLocation.Y,\
        StateQueueIndexLocation.Z,\
        StateQueueIndexRotation.Roll,\
        StateQueueIndexRotation.Pitch,\
        StateQueueIndexRotation.Yaw,\
        StateQueueIndexControlRotation.Roll,\
        StateQueueIndexControlRotation.Pitch,\
        StateQueueIndexControlRotation.Yaw,\
        StateQueueIndex.bInputFlag1, StateQueueIndex.bInputFlag2, StateQueueIndex.bInputFlag3, StateQueueIndex.bInputFlag4,\
        StateQueueIndex.bInputFlag5, StateQueueIndex.bInputFlag6, StateQueueIndex.bInputFlag7, StateQueueIndex.bInputFlag8,\
        StateQueueIndex.bInputFlag9, StateQueueIndex.bInputFlag10, StateQueueIndex.bInputFlag11, StateQueueIndex.bInputFlag12,\
        StateQueueIndex.bInputFlag13, StateQueueIndex.bInputFlag14, StateQueueIndex.bInputFlag15, StateQueueIndex.bInputFlag16)\
    }\
    GMC_LOG(Log,\
    TEXT("SmoothingLog: --- State Queue Contents END ---"))\
  }

// Log the states passed to the simulated tick function.
#define DEBUG_LOG_SIMULATED_TICK_ARGUMENTS\
  if (GMCCVars::LogSimulatedTickData == 1 && PawnOwner->GetLocalRole() == ROLE_Authority\
    || GMCCVars::LogSimulatedTickData >= 2 && PawnOwner->GetLocalRole() == ROLE_SimulatedProxy) {\
    GMC_LOG(Log, TEXT("SimulatedTickLog: --- Simulated Tick Arguments BEGIN ---"))\
    if (IsValidStateQueueIndex(CurrentStartStateIndex) && IsValidStateQueueIndex(CurrentTargetStateIndex)) {\
      const FState& StartState = AccessStateQueue(CurrentStartStateIndex);\
      const FVector StartStateVelocity = StartState.Velocity;\
      const FVector StartStateLocation = StartState.Location;\
      const FRotator StartStateRotation = StartState.Rotation;\
      const FRotator StartStateControlRotation = StartState.ControlRotation;\
      const FState& TargetState = AccessStateQueue(CurrentTargetStateIndex);\
      const FVector TargetStateVelocity = TargetState.Velocity;\
      const FVector TargetStateLocation = TargetState.Location;\
      const FRotator TargetStateRotation = TargetState.Rotation;\
      const FRotator TargetStateControlRotation = TargetState.ControlRotation;\
      GMC_LOG(Log,\
        TEXT("SimulatedTickLog: StateQueue[StartStateIndex  = %3d]. Timestamp = %10.3f | Velocity = % 12.3f, % 12.3f, % 12.3f | Location = % 12.3f, % 12.3f, % 12.3f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"),\
        CurrentStartStateIndex,\
        StartState.Timestamp,\
        StartStateVelocity.X,\
        StartStateVelocity.Y,\
        StartStateVelocity.Z,\
        StartStateLocation.X,\
        StartStateLocation.Y,\
        StartStateLocation.Z,\
        StartState.bInputFlag1, StartState.bInputFlag2, StartState.bInputFlag3, StartState.bInputFlag4,\
        StartState.bInputFlag5, StartState.bInputFlag6, StartState.bInputFlag7, StartState.bInputFlag8,\
        StartState.bInputFlag9, StartState.bInputFlag10, StartState.bInputFlag11, StartState.bInputFlag12,\
        StartState.bInputFlag13, StartState.bInputFlag14, StartState.bInputFlag15, StartState.bInputFlag16)\
      GMC_LOG(Log,\
        TEXT("SimulatedTickLog: StateQueue[TargetStateIndex = %3d]. Timestamp = %10.3f | Velocity = % 12.3f, % 12.3f, % 12.3f | Location = % 12.3f, % 12.3f, % 12.3f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"),\
        CurrentTargetStateIndex,\
        TargetState.Timestamp,\
        TargetStateVelocity.X,\
        TargetStateVelocity.Y,\
        TargetStateVelocity.Z,\
        TargetStateLocation.X,\
        TargetStateLocation.Y,\
        TargetStateLocation.Z,\
        TargetState.bInputFlag1, TargetState.bInputFlag2, TargetState.bInputFlag3, TargetState.bInputFlag4,\
        TargetState.bInputFlag5, TargetState.bInputFlag6, TargetState.bInputFlag7, TargetState.bInputFlag8,\
        TargetState.bInputFlag9, TargetState.bInputFlag10, TargetState.bInputFlag11, TargetState.bInputFlag12,\
        TargetState.bInputFlag13, TargetState.bInputFlag14, TargetState.bInputFlag15, TargetState.bInputFlag16)\
      if (SkippedStateIndices.Num() > 0) {\
        for (const auto SkippedStateIndex : SkippedStateIndices) {\
          const auto& SkippedState = AccessStateQueue(SkippedStateIndex);\
          const FVector SkippedStateVelocity = SkippedState.Velocity;\
          const FVector SkippedStateLocation = SkippedState.Location;\
          const FRotator SkippedStateRotation = SkippedState.Rotation;\
          const FRotator SkippedStateControlRotation = SkippedState.ControlRotation;\
          GMC_LOG(Log,\
            TEXT("SimulatedTickLog: SkippedState[Index          = %3d]. Timestamp = %10.3f | Velocity = % 12.3f, % 12.3f, % 12.3f | Location = % 12.3f, % 12.3f, % 12.3f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"),\
            SkippedStateIndex,\
            SkippedState.Timestamp,\
            SkippedStateVelocity.X,\
            SkippedStateVelocity.Y,\
            SkippedStateVelocity.Z,\
            SkippedStateLocation.X,\
            SkippedStateLocation.Y,\
            SkippedStateLocation.Z,\
            SkippedState.bInputFlag1, SkippedState.bInputFlag2, SkippedState.bInputFlag3, SkippedState.bInputFlag4,\
            SkippedState.bInputFlag5, SkippedState.bInputFlag6, SkippedState.bInputFlag7, SkippedState.bInputFlag8,\
            SkippedState.bInputFlag9, SkippedState.bInputFlag10, SkippedState.bInputFlag11, SkippedState.bInputFlag12,\
            SkippedState.bInputFlag13, SkippedState.bInputFlag14, SkippedState.bInputFlag15, SkippedState.bInputFlag16)\
        }\
      }\
    }\
    else {\
      GMC_LOG(Log,\
        TEXT("SimulatedTickLog: StateQueue[StartStateIndex  = %3d]"),\
        CurrentStartStateIndex)\
      GMC_LOG(Log,\
        TEXT("SimulatedTickLog: StateQueue[TargetStateIndex = %3d]"),\
        CurrentTargetStateIndex)\
      checkGMC(SkippedStateIndices.Num() == 0)\
    }\
    GMC_LOG(Log, TEXT("SimulatedTickLog: --- Simulated Tick Arguments END ---"))\
  }

// Trace the values of a move from the client to the server back to the client.
#define DEBUG_LOG_CLIENT_EXECUTED_MOVE\
  if (GMCCVars::LogClientMoveTrace != 0) {\
    GMC_LOG(Log,\
        TEXT("TrackClientMove: Client move execution result (raw)         : Timestamp = %12.6f (ClientWorldTime = %12.6f) | InputVector = % 9.6f, % 9.6f, % 9.6f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) | InVelocity(NoRep) = % 15.6f, % 15.6f, % 15.6f | InLocation(NoRep) = % 15.6f, % 15.6f, % 15.6f | InRotation(NoRep) = % 11.6f, % 11.6f, % 11.6f | InControlRotation(NoRep) = % 11.6f, % 11.6f, % 11.6f | OutVelocity(NoRep) = % 15.6f, % 15.6f, % 15.6f | OutLocation = % 15.6f, % 15.6f, % 15.6f | OutRotation = % 11.6f, % 11.6f, % 11.6f | OutControlRotation = % 11.6f, % 11.6f, % 11.6f |"),\
        Move.Timestamp,\
        GetTime(),\
        Move.InputVector.X, Move.InputVector.Y, Move.InputVector.Z,\
        Move.bInputFlag1, Move.bInputFlag2, Move.bInputFlag3, Move.bInputFlag4,\
        Move.bInputFlag5, Move.bInputFlag6, Move.bInputFlag7, Move.bInputFlag8,\
        Move.bInputFlag9, Move.bInputFlag10, Move.bInputFlag11, Move.bInputFlag12,\
        Move.bInputFlag13, Move.bInputFlag14, Move.bInputFlag15, Move.bInputFlag16,\
        Move.InVelocity.X, Move.InVelocity.Y, Move.InVelocity.Z,\
        Move.InLocation.X, Move.InLocation.Y, Move.InLocation.Z,\
        Move.InRotation.Roll, Move.InRotation.Pitch, Move.InRotation.Yaw,\
        Move.InControlRotation.Roll, Move.InControlRotation.Pitch, Move.InControlRotation.Yaw,\
        Move.OutVelocity.X, Move.OutVelocity.Y, Move.OutVelocity.Z,\
        Move.OutLocation.X, Move.OutLocation.Y, Move.OutLocation.Z,\
        Move.OutRotation.Roll, Move.OutRotation.Pitch, Move.OutRotation.Yaw,\
        Move.OutControlRotation.Roll, Move.OutControlRotation.Pitch, Move.OutControlRotation.Yaw)\
  }
#define DEBUG_LOG_CLIENT_SENT_MOVES\
  if (GMCCVars::LogClientMoveTrace != 0) {\
    int32 NumMove = 0;\
    for (const auto& SentMove : Client_PendingMoves) {\
      ++NumMove;\
      GMC_LOG(Log,\
        TEXT("TrackClientMove: Client sent move (quantized)            %2d : Timestamp = %12.6f (ClientWorldTime = %12.6f) | InputVector = % 9.6f, % 9.6f, % 9.6f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) | InVelocity(NoRep) = % 15.6f, % 15.6f, % 15.6f | InLocation(NoRep) = % 15.6f, % 15.6f, % 15.6f | InRotation(NoRep) = % 11.6f, % 11.6f, % 11.6f | InControlRotation(NoRep) = % 11.6f, % 11.6f, % 11.6f | OutVelocity(NoRep) = % 15.6f, % 15.6f, % 15.6f | OutLocation = % 15.6f, % 15.6f, % 15.6f | OutRotation = % 11.6f, % 11.6f, % 11.6f | OutControlRotation = % 11.6f, % 11.6f, % 11.6f |"),\
        NumMove,\
        SentMove.Timestamp,\
        GetTime(),\
        SentMove.InputVector.X, SentMove.InputVector.Y, SentMove.InputVector.Z,\
        SentMove.bInputFlag1, SentMove.bInputFlag2, SentMove.bInputFlag3, SentMove.bInputFlag4,\
        SentMove.bInputFlag5, SentMove.bInputFlag6, SentMove.bInputFlag7, SentMove.bInputFlag8,\
        SentMove.bInputFlag9, SentMove.bInputFlag10, SentMove.bInputFlag11, SentMove.bInputFlag12,\
        SentMove.bInputFlag13, SentMove.bInputFlag14, SentMove.bInputFlag15, SentMove.bInputFlag16,\
        SentMove.InVelocity.X, SentMove.InVelocity.Y, SentMove.InVelocity.Z,\
        SentMove.InLocation.X, SentMove.InLocation.Y, SentMove.InLocation.Z,\
        SentMove.InRotation.Roll, SentMove.InRotation.Pitch, SentMove.InRotation.Yaw,\
        SentMove.InControlRotation.Roll, SentMove.InControlRotation.Pitch, SentMove.InControlRotation.Yaw,\
        SentMove.OutVelocity.X, SentMove.OutVelocity.Y, SentMove.OutVelocity.Z,\
        SentMove.OutLocation.X, SentMove.OutLocation.Y, SentMove.OutLocation.Z,\
        SentMove.OutRotation.Roll, SentMove.OutRotation.Pitch, SentMove.OutRotation.Yaw,\
        SentMove.OutControlRotation.Roll, SentMove.OutControlRotation.Pitch, SentMove.OutControlRotation.Yaw)\
    }\
  }
#define DEBUG_LOG_SERVER_RECEIVED_MOVES\
  if (GMCCVars::LogClientMoveTrace != 0) {\
    GMC_LOG(Log,\
        TEXT("TrackClientMove: Server received move (quantized)        %2d : Timestamp = %12.6f (ServerWorldTime = %12.6f) | InputVector = % 9.6f, % 9.6f, % 9.6f | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) | InVelocity(NoRep) = % 15.6f, % 15.6f, % 15.6f | InLocation(NoRep) = % 15.6f, % 15.6f, % 15.6f | InRotation(NoRep) = % 11.6f, % 11.6f, % 11.6f | InControlRotation(NoRep) = % 11.6f, % 11.6f, % 11.6f | OutVelocity(NoRep) = % 15.6f, % 15.6f, % 15.6f | OutLocation = % 15.6f, % 15.6f, % 15.6f | OutRotation = % 11.6f, % 11.6f, % 11.6f | OutControlRotation = % 11.6f, % 11.6f, % 11.6f |"),\
        Index + 1,\
        ClientMove.Timestamp,\
        GetTime(),\
        ClientMove.InputVector.X, ClientMove.InputVector.Y, ClientMove.InputVector.Z,\
        ClientMove.bInputFlag1, ClientMove.bInputFlag2, ClientMove.bInputFlag3, ClientMove.bInputFlag4,\
        ClientMove.bInputFlag5, ClientMove.bInputFlag6, ClientMove.bInputFlag7, ClientMove.bInputFlag8,\
        ClientMove.bInputFlag9, ClientMove.bInputFlag10, ClientMove.bInputFlag11, ClientMove.bInputFlag12,\
        ClientMove.bInputFlag13, ClientMove.bInputFlag14, ClientMove.bInputFlag15, ClientMove.bInputFlag16,\
        ClientMove.InVelocity.X, ClientMove.InVelocity.Y, ClientMove.InVelocity.Z,\
        ClientMove.InLocation.X, ClientMove.InLocation.Y, ClientMove.InLocation.Z,\
        ClientMove.InRotation.Roll, ClientMove.InRotation.Pitch, ClientMove.InRotation.Yaw,\
        ClientMove.InControlRotation.Roll, ClientMove.InControlRotation.Pitch, ClientMove.InControlRotation.Yaw,\
        ClientMove.OutVelocity.X, ClientMove.OutVelocity.Y, ClientMove.OutVelocity.Z,\
        ClientMove.OutLocation.X, ClientMove.OutLocation.Y, ClientMove.OutLocation.Z,\
        ClientMove.OutRotation.Roll, ClientMove.OutRotation.Pitch, ClientMove.OutRotation.Yaw,\
        ClientMove.OutControlRotation.Roll, ClientMove.OutControlRotation.Pitch, ClientMove.OutControlRotation.Yaw)\
  }
#define DEBUG_LOG_SERVER_EXECUTED_MOVE_RAW\
  if (GMCCVars::LogClientMoveTrace != 0) {\
    const FVector PawnVelocity = GetVelocity();\
    const FVector PawnLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
        TEXT("TrackClientMove: Server move execution result (raw)      %2d : Timestamp = %12.6f (ServerWorldTime = %12.6f) |                                                                                                                                                                                                                                                                                                                     %*s | OutVelocity        = % 15.6f, % 15.6f, % 15.6f | OutLocation = % 15.6f, % 15.6f, % 15.6f | OutRotation = % 11.6f, % 11.6f, % 11.6f | OutControlRotation = % 11.6f, % 11.6f, % 11.6f |"),\
        Index + 1,\
        ClientMove.Timestamp,\
        GetTime(),\
        68, TEXT(" "),\
        PawnVelocity.X, PawnVelocity.Y, PawnVelocity.Z,\
        PawnLocation.X, PawnLocation.Y, PawnLocation.Z,\
        PawnRotation.Roll, PawnRotation.Pitch, PawnRotation.Yaw,\
        PawnControlRotation.Roll, PawnControlRotation.Pitch, PawnControlRotation.Yaw)\
  }
#define DEBUG_LOG_SERVER_EXECUTED_MOVE_RESOLVED\
  if (GMCCVars::LogClientMoveTrace != 0) {\
    const FVector PawnVelocity = GetVelocity();\
    const FVector PawnLocation = PawnOwner->GetActorLocation();\
    const FRotator PawnRotation = PawnOwner->GetActorRotation();\
    const FRotator PawnControlRotation = PawnOwner->GetControlRotation();\
    GMC_LOG(Log,\
        TEXT("TrackClientMove: Server move execution result (resolved) %2d : Timestamp = %12.6f (ServerWorldTime = %12.6f) |                                                                                                                                                                                                                                                                                                                     %*s | OutVelocity        = % 15.6f, % 15.6f, % 15.6f | OutLocation = % 15.6f, % 15.6f, % 15.6f | OutRotation = % 11.6f, % 11.6f, % 11.6f | OutControlRotation = % 11.6f, % 11.6f, % 11.6f |"),\
        Index + 1,\
        ClientMove.Timestamp,\
        GetTime(),\
        68, TEXT(" "),\
        PawnVelocity.X, PawnVelocity.Y, PawnVelocity.Z,\
        PawnLocation.X, PawnLocation.Y, PawnLocation.Z,\
        PawnRotation.Roll, PawnRotation.Pitch, PawnRotation.Yaw,\
        PawnControlRotation.Roll, PawnControlRotation.Pitch, PawnControlRotation.Yaw)\
  }
#define DEBUG_LOG_SERVER_SENT_STATE_TO_AUTONOMOUS_PROXY\
  if (GMCCVars::LogClientMoveTrace != 0) {\
      const auto& ServerState = ServerState_AutonomousProxy();\
      GMC_LOG(Log,\
        TEXT("TrackClientMove: Server sent state (quantized)              : Timestamp = %12.6f (ServerWorldTime = %12.6f) |                                  %*s | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) |                                                                                                                                                                                                   %*s | Velocity           = % 15.6f, % 15.6f, % 15.6f | Location    = % 15.6f, % 15.6f, % 15.6f | Rotation    = % 11.6f, % 11.6f, % 11.6f | ControlRotation    = % 11.6f, % 11.6f, % 11.6f |"),\
        ServerState.Timestamp,\
        GetTime(),\
        12, TEXT(" "),\
        ServerState.bInputFlag1, ServerState.bInputFlag2, ServerState.bInputFlag3, ServerState.bInputFlag4,\
        ServerState.bInputFlag5, ServerState.bInputFlag6, ServerState.bInputFlag7, ServerState.bInputFlag8,\
        ServerState.bInputFlag9, ServerState.bInputFlag10, ServerState.bInputFlag11, ServerState.bInputFlag12,\
        ServerState.bInputFlag13, ServerState.bInputFlag14, ServerState.bInputFlag15, ServerState.bInputFlag16,\
        74, TEXT(" "),\
        ServerState.Velocity.X, ServerState.Velocity.Y, ServerState.Velocity.Z,\
        ServerState.Location.X, ServerState.Location.Y, ServerState.Location.Z,\
        ServerState.Rotation.Roll, ServerState.Rotation.Pitch, ServerState.Rotation.Yaw,\
        ServerState.ControlRotation.Roll, ServerState.ControlRotation.Pitch, ServerState.ControlRotation.Yaw)\
  }
#define DEBUG_LOG_CLIENT_RECEIVED_STATE\
  if (GMCCVars::LogClientMoveTrace != 0) {\
      const auto& ServerState = ServerState_AutonomousProxy();\
      GMC_LOG(Log,\
        TEXT("TrackClientMove: Client received state (quantized)          : Timestamp = %12.6f (ClientWorldTime = %12.6f) |                                  %*s | bFlags = (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) |                                                                                                                                                                                                   %*s | Velocity           = % 15.6f, % 15.6f, % 15.6f | Location    = % 15.6f, % 15.6f, % 15.6f | Rotation    = % 11.6f, % 11.6f, % 11.6f | ControlRotation    = % 11.6f, % 11.6f, % 11.6f |"),\
        ServerState.Timestamp,\
        GetTime(),\
        12, TEXT(" "),\
        ServerState.bInputFlag1, ServerState.bInputFlag2, ServerState.bInputFlag3, ServerState.bInputFlag4,\
        ServerState.bInputFlag5, ServerState.bInputFlag6, ServerState.bInputFlag7, ServerState.bInputFlag8,\
        ServerState.bInputFlag9, ServerState.bInputFlag10, ServerState.bInputFlag11, ServerState.bInputFlag12,\
        ServerState.bInputFlag13, ServerState.bInputFlag14, ServerState.bInputFlag15, ServerState.bInputFlag16,\
        74, TEXT(" "),\
        ServerState.Velocity.X, ServerState.Velocity.Y, ServerState.Velocity.Z,\
        ServerState.Location.X, ServerState.Location.Y, ServerState.Location.Z,\
        ServerState.Rotation.Roll, ServerState.Rotation.Pitch, ServerState.Rotation.Yaw,\
        ServerState.ControlRotation.Roll, ServerState.ControlRotation.Pitch, ServerState.ControlRotation.Yaw)\
  }

#else

#define DEBUG_PRINT_LOCAL_STATS(PawnNetRole)
#define DEBUG_DISPLAY_LOCAL_STATS
#define DEBUG_PRINT_NET_STATS(PawnNetRole)
#define DEBUG_DISPLAY_NET_STATS
#define DEBUG_SHOW_PAWN_NET_ROLE
#define DEBUG_NET_CORRECTION_ORIGINAL_CLIENT_LOCATION
#define DEBUG_NET_CORRECTION_UPDATED_CLIENT_LOCATION
#define DEBUG_NET_CORRECTION_REPLAYED_CLIENT_LOCATION
#define DEBUG_NET_CORRECTION_DRAW_CLIENT_SHAPES
#define DEBUG_SHOW_CLIENT_LOCATION_ERRORS_ON_SERVER
#define DEBUG_LOG_MOVE_BEFORE_ITERATION
#define DEBUG_LOG_MOVE_AFTER_ITERATION
#define DEBUG_LOG_MOVE_QUEUE_SIZE_BEFORE_CLEARING
#define DEBUG_LOG_MOVE_QUEUE_SIZE_AFTER_CLEARING
#define DEBUG_LOG_REPLAY_CLIENT_STATE_INITIAL
#define DEBUG_LOG_REPLAY_SERVER_STATE
#define DEBUG_LOG_REPLAY_CLIENT_STATE_BEFORE_REPLAY
#define DEBUG_LOG_REPLAY_CLIENT_REPLAY_LOOP
#define DEBUG_LOG_REPLAY_CLIENT_STATE_AFTER_REPLAY
#define DEBUG_LOG_SMOOTHING_INTERPOLATION_DATA
#define DEBUG_LOG_STATE_QUEUE_DATA
#define DEBUG_LOG_SIMULATED_TICK_ARGUMENTS
#define DEBUG_LOG_CLIENT_EXECUTED_MOVE
#define DEBUG_LOG_CLIENT_SENT_MOVES
#define DEBUG_LOG_SERVER_RECEIVED_MOVES
#define DEBUG_LOG_SERVER_EXECUTED_MOVE_RAW
#define DEBUG_LOG_SERVER_EXECUTED_MOVE_RESOLVED
#define DEBUG_LOG_SERVER_SENT_STATE_TO_AUTONOMOUS_PROXY
#define DEBUG_LOG_CLIENT_RECEIVED_STATE

#endif
