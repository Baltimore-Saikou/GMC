// Copyright 2022 Dominik Scherer. All Rights Reserved.

#include "GenMovementComponent.h"
#include "FlatCapsuleComponent.h"
#define GMC_MOVEMENT_COMPONENT_LOG
#include "GMC_LOG.h"
#include "GenMovementComponent_DBG.h"

DEFINE_LOG_CATEGORY(LogGMCMovement)

DECLARE_CYCLE_STAT(TEXT("Replicated Tick"), STAT_ReplicatedTick, STATGROUP_GMCGenMovementComp)
DECLARE_CYCLE_STAT(TEXT("Simulated Tick"), STAT_SimulatedTick, STATGROUP_GMCGenMovementComp)
DECLARE_CYCLE_STAT(TEXT("Update Floor"), STAT_UpdateFloor, STATGROUP_GMCGenMovementComp)
DECLARE_CYCLE_STAT(TEXT("Compute Immersion Depth"), STAT_ComputeImmersionDepth, STATGROUP_GMCGenMovementComp)
DECLARE_CYCLE_STAT(TEXT("Set Root Collision Shape"), STAT_SetRootCollisionShape, STATGROUP_GMCGenMovementComp)
DECLARE_CYCLE_STAT(TEXT("Set Root Collision Extent"), STAT_SetRootCollisionExtent, STATGROUP_GMCGenMovementComp)
DECLARE_CYCLE_STAT(TEXT("Is Valid Position"), STAT_IsValidPosition, STATGROUP_GMCGenMovementComp)

namespace GMCCVars
{
#if ALLOW_CONSOLE && !NO_LOGGING

  int32 ShowMovementVectors = 0;
  FAutoConsoleVariableRef CVarShowMovementVectors(
    TEXT("gmc.ShowMovementVectors"),
    ShowMovementVectors,
    TEXT("Visualize velocity and acceleration vectors. 0: Disable, 1: Enable"),
    ECVF_Default
  );

#endif
}

void UGenMovementComponent::ReplicatedTick(const FMove& Move, int32 Iteration, bool bIsSubSteppedIteration)
{
  SCOPE_CYCLE_COUNTER(STAT_ReplicatedTick)

  // Save the values of the current move into the local members.
  Timestamp = Move.Timestamp;
  MoveDeltaTime = Move.DeltaTime;
  InputVector = Move.InputVector;
  InVelocity = Move.InVelocity;
  InLocation = Move.InLocation;
  InRotation = Move.InRotation;
  InControlRotation = Move.InControlRotation;
  InInputMode = Move.InInputMode;
  OutVelocity = Move.OutVelocity;
  OutLocation = Move.OutLocation;
  OutRotation = Move.OutRotation;
  OutControlRotation = Move.OutControlRotation;
  OutInputMode = Move.OutInputMode;
  MoveIteration = Iteration;
  bIsSubSteppedMoveIteration = bIsSubSteppedIteration;

  // Reset physics values.
  SetPhysDeltaTime(MoveDeltaTime);

  // Simplified tick function for subclasses.
  GenReplicatedTick(MoveDeltaTime);
  DEBUG_GMC_SHOW_MOVEMENT_VECTORS
}

void UGenMovementComponent::SimulatedTick(
  float DeltaTime,
  const FState& SmoothState,
  int32 StartStateIndex,
  int32 TargetStateIndex,
  const TArray<int32>& SkippedStateIndices
)
{
  SCOPE_CYCLE_COUNTER(STAT_SimulatedTick)

  // Set the member pointers for the current interpolation iteration.
  if (IsValidStateQueueIndex(StartStateIndex) && IsValidStateQueueIndex(TargetStateIndex))
  {
    StartStatePtr = &AccessStateQueue(StartStateIndex);
    TargetStatePtr = &AccessStateQueue(TargetStateIndex);
    SkippedStatePtrs.Reset();
    for (const auto SkippedStateIndex : SkippedStateIndices)
    {
      SkippedStatePtrs.Emplace(&AccessStateQueue(SkippedStateIndex));
    }
  }
  else
  {
    // No valid states for this iteration.
    StartStatePtr = nullptr;
    TargetStatePtr = nullptr;
    SkippedStatePtrs.Reset();
  }

  // Simplified tick function for subclasses.
  GenSimulatedTick(DeltaTime);
}

const FState& UGenMovementComponent::GetCurrentInterpolationStartState() const
{
  if (StartStatePtr)
  {
    return *StartStatePtr;
  }
  return DefaultState;
}

const FState& UGenMovementComponent::GetCurrentInterpolationTargetState() const
{
  if (TargetStatePtr)
  {
    return *TargetStatePtr;
  }
  return DefaultState;
}

TArray<FState> UGenMovementComponent::GetSkippedInterpolationStates() const
{
  TArray<FState> RetSkippedStates;
  if (SkippedStatePtrs.Num() > 0)
  {
    // We need to copy into an array of actual states here to have this function be callable from Blueprint. However, there won't be any
    // skipped states most of the time so this is not a big performance hog even if done every frame.
    for (const auto SkippedStatePtr : SkippedStatePtrs)
    {
      RetSkippedStates.Emplace(*SkippedStatePtr);
    }
  }
  return RetSkippedStates;
}

float UGenMovementComponent::GetPhysDeltaTime() const
{
  return PhysDeltaTime;
}

void UGenMovementComponent::UpdateVelocity(const FVector& NewVelocity, float DeltaTime)
{
  if (NewVelocity != Velocity)
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      Velocity = NewVelocity;
      Acceleration = (Velocity - Velocity0) / DeltaTime;
      Force = Mass * Acceleration;
    }
  }
}

void UGenMovementComponent::UpdateAcceleration(const FVector& NewAcceleration, float DeltaTime)
{
  if (NewAcceleration != Acceleration)
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      Acceleration = NewAcceleration;
      Velocity = Velocity0 + Acceleration * DeltaTime;
      Force = Mass * Acceleration;
    }
  }
}

void UGenMovementComponent::UpdateForce(const FVector& NewForce, float DeltaTime)
{
  if (NewForce != Force)
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      checkGMC(Mass >= KINDA_SMALL_NUMBER)
      Force = NewForce;
      Velocity = Velocity0 + Force * DeltaTime / FMath::Max(Mass, KINDA_SMALL_NUMBER);
      Acceleration = (Velocity - Velocity0) / DeltaTime;
    }
  }
}

void UGenMovementComponent::AddVelocity(const FVector& AddVelocity, float DeltaTime)
{
  if (!AddVelocity.IsZero())
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      Velocity += AddVelocity * DeltaTime;
      const FVector AddAcceleration = AddVelocity / DeltaTime;
      Acceleration += AddAcceleration;
      Force += AddAcceleration * Mass;
    }
  }
}

void UGenMovementComponent::AddAcceleration(const FVector& AddAcceleration, float DeltaTime)
{
  if (!AddAcceleration.IsZero())
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      Acceleration += AddAcceleration;
      Velocity += AddAcceleration * DeltaTime;
      Force += AddAcceleration * Mass;
    }
  }
}

void UGenMovementComponent::AddForce(const FVector& AddForce, float DeltaTime)
{
  if (!AddForce.IsZero())
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      checkGMC(Mass >= KINDA_SMALL_NUMBER)
      Force += AddForce;
      const FVector AddVelocity = AddForce * DeltaTime / FMath::Max(Mass, KINDA_SMALL_NUMBER);
      Velocity += AddVelocity;
      Acceleration = AddVelocity / DeltaTime;
    }
  }
}

void UGenMovementComponent::AddImpulse(const FVector& Impulse, bool bVelChange)
{
  if (!Impulse.IsZero())
  {
    if (!bVelChange)
    {
      checkGMC(Mass >= KINDA_SMALL_NUMBER)
      AddVelocity(Impulse / FMath::Max(Mass, KINDA_SMALL_NUMBER), 1.f);
    }
    else
    {
      // No scaling by mass i.e. mass is assumed to be 1 kg.
      AddVelocity(Impulse, 1.f);
    }
  }
}

void UGenMovementComponent::AddRadialForce(
  const FVector& Origin,
  float Radius,
  float Strength,
  EGenRadialImpulseFalloff Falloff,
  float DeltaTime
)
{
  if (Radius > 0.f && Strength > 0.f)
  {
    DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
    if (DeltaTime >= MIN_DELTA_TIME)
    {
      const FVector Delta = UpdatedComponent->GetComponentLocation() - Origin;
      const float DistanceFromSource = Delta.Size();
      if (DistanceFromSource > Radius)
      {
        // We are not affected if we are outside of the force radius.
        return;
      }

      float EffectiveStrength{0.f};
      if (Falloff == EGenRadialImpulseFalloff::Linear && Radius > KINDA_SMALL_NUMBER)
      {
        // Falloff is not constant meaning <Strength> is the max strength of the force at the point of origin and drops linearly with the
        // distance from the origin.
        EffectiveStrength = Strength * (1.f - (DistanceFromSource / Radius));
      }
      const FVector ForceDirection = Delta.GetSafeNormal();
      AddForce(ForceDirection * EffectiveStrength, DeltaTime);
    }
  }
}

void UGenMovementComponent::AddRadialForce(const FVector& Origin, float Radius, float Strength, ERadialImpulseFalloff Falloff)
{
  AddRadialForce(Origin, Radius, Strength, static_cast<EGenRadialImpulseFalloff>(Falloff), GetPhysDeltaTime());
}

void UGenMovementComponent::AddRadialImpulse(
  const FVector& Origin,
  float Radius,
  float Strength,
  EGenRadialImpulseFalloff Falloff,
  bool bVelChange
)
{
  if (Radius > 0.f && Strength > 0.f)
  {
    const FVector Delta = UpdatedComponent->GetComponentLocation() - Origin;
    const float DistanceFromSource = Delta.Size();
    if (DistanceFromSource > Radius)
    {
      // We are not affected if we are outside of the impulse radius.
      return;
    }

    float EffectiveStrength{0.f};
    if (Falloff == EGenRadialImpulseFalloff::Linear && Radius > KINDA_SMALL_NUMBER)
    {
      // Falloff is not constant meaning <Strength> is the max strength of the impulse at the point of origin and drops linearly with the
      // distance from the origin.
      EffectiveStrength = Strength * (1.f - (DistanceFromSource / Radius));
    }
    const FVector ImpulseDirection = Delta.GetSafeNormal();
    AddImpulse(ImpulseDirection * EffectiveStrength, bVelChange);
  }
}

void UGenMovementComponent::AddRadialImpulse(
  const FVector& Origin,
  float Radius,
  float Strength,
  ERadialImpulseFalloff Falloff,
  bool bVelChange
)
{
  AddRadialImpulse(Origin, Radius, Strength, static_cast<EGenRadialImpulseFalloff>(Falloff), bVelChange);
}

void UGenMovementComponent::SetPhysDeltaTime(float NewDeltaTime, bool bReset)
{
  PhysDeltaTime = NewDeltaTime;
  if (bReset)
  {
    Location0 = UpdatedComponent->GetComponentLocation();
    Velocity0 = Velocity;
    Acceleration = FVector::ZeroVector;
    Force = FVector::ZeroVector;
  }
}

void UGenMovementComponent::UpdateVelocityFromMovedDistance(float DeltaTime, float MaxSize)
{
  DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
  if (DeltaTime >= MIN_DELTA_TIME)
  {
    UpdateVelocity((UpdatedComponent->GetComponentLocation() - Location0) / DeltaTime, DeltaTime);
    if (MaxSize > 0.f) UpdateVelocity(Velocity.GetClampedToMaxSize(MaxSize), DeltaTime);
  }
}

bool UGenMovementComponent::HasMoved() const
{
  return !UpdatedComponent->GetComponentLocation().Equals(Location0);
}

void UGenMovementComponent::AdjustVelocityFromHit(const FHitResult& Hit, float DeltaTime)
{
  if (!Hit.IsValidBlockingHit())
  {
    return;
  }
  FVector Normal = Hit.Normal;
  // The hit normal can differ slightly between client and server which can lead to more significant errors when projecting the velocity.
  // To mitigate this problem we round the normal to 2 decimal places to get consistent results across client and server.
  Normal.X = FMath::RoundToFloat(Normal.X * 100.f) / 100.f;
  Normal.Y = FMath::RoundToFloat(Normal.Y * 100.f) / 100.f;
  Normal.Z = FMath::RoundToFloat(Normal.Z * 100.f) / 100.f;
  if ((Velocity | Normal) <= 0.f)
  {
    // When colliding with something, we need to adjust the velocity into a direction that does not go into the hit object. We also adjust
    // the saved start velocity here to avoid artifacts in the acceleration calculation.
    Velocity0 = FVector::VectorPlaneProject(Velocity0, Normal);
    UpdateVelocity(FVector::VectorPlaneProject(Velocity, Normal), DeltaTime);
  }
}

void UGenMovementComponent::AdjustVelocityFromNormal(FVector Normal, float DeltaTime)
{
  Normal = Normal.GetSafeNormal();
  if (Normal.IsZero())
  {
    return;
  }
  // A hit normal (which is usually used for this) can differ slightly between client and server which can lead to more significant errors
  // when projecting the velocity. To mitigate this problem we round the normal to 2 decimal places to get consistent results across client
  // and server.
  Normal.X = FMath::RoundToFloat(Normal.X * 100.f) / 100.f;
  Normal.Y = FMath::RoundToFloat(Normal.Y * 100.f) / 100.f;
  Normal.Z = FMath::RoundToFloat(Normal.Z * 100.f) / 100.f;
  if ((Velocity | Normal) <= 0.f)
  {
    // When colliding with something, we need to adjust the velocity into a direction that does not go into the hit object. We also adjust
    // the saved start velocity here to avoid artifacts in the acceleration calculation.
    Velocity0 = FVector::VectorPlaneProject(Velocity0, Normal);
    UpdateVelocity(FVector::VectorPlaneProject(Velocity, Normal), DeltaTime);
  }
}

float UGenMovementComponent::GenerateNetRandomNumber(int32 Max) const
{
  if (Max > 0) {
    constexpr double Factor = 99999989/*prime*/;
    return (float)std::fmod((double)GetMoveTimestamp() * Factor, (double)Max);
  }
  return 0.f;
}

float UGenMovementComponent::GenerateNetRandomAngle() const
{
  constexpr double Factor = 99991/*prime*/;
  return (float)std::fmod((double)GetMoveTimestamp() * Factor, (double)360);
}

float UGenMovementComponent::RoundFloat(const float& FloatToRound, EDecimalQuantization Level)
{
  float RoundedFloat = FloatToRound;
  switch (Level)
  {
    case EDecimalQuantization::RoundTwoDecimals:
    {
      RoundedFloat = FMath::RoundToFloat(FloatToRound * 100.f) / 100.f;
      return RoundedFloat;
    }
    case EDecimalQuantization::RoundOneDecimal:
    {
      RoundedFloat = FMath::RoundToFloat(FloatToRound * 10.f) / 10.f;
      return RoundedFloat;
    }
    case EDecimalQuantization::RoundWholeNumber:
    {
      RoundedFloat = FMath::RoundToFloat(FloatToRound);
      return RoundedFloat;
    }
    case EDecimalQuantization::None:
    {
      return RoundedFloat;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return 0.f;
}

FVector UGenMovementComponent::RoundVector(const FVector& VectorToRound, EDecimalQuantization Level)
{
  FVector RoundedVector = VectorToRound;
  switch (Level)
  {
    case EDecimalQuantization::RoundTwoDecimals:
    {
      RoundedVector.X = FMath::RoundToFloat(VectorToRound.X * 100.f) / 100.f;
      RoundedVector.Y = FMath::RoundToFloat(VectorToRound.Y * 100.f) / 100.f;
      RoundedVector.Z = FMath::RoundToFloat(VectorToRound.Z * 100.f) / 100.f;
      return RoundedVector;
    }
    case EDecimalQuantization::RoundOneDecimal:
    {
      RoundedVector.X = FMath::RoundToFloat(VectorToRound.X * 10.f) / 10.f;
      RoundedVector.Y = FMath::RoundToFloat(VectorToRound.Y * 10.f) / 10.f;
      RoundedVector.Z = FMath::RoundToFloat(VectorToRound.Z * 10.f) / 10.f;
      return RoundedVector;
    }
    case EDecimalQuantization::RoundWholeNumber:
    {
      RoundedVector.X = FMath::RoundToFloat(VectorToRound.X);
      RoundedVector.Y = FMath::RoundToFloat(VectorToRound.Y);
      RoundedVector.Z = FMath::RoundToFloat(VectorToRound.Z);
      return RoundedVector;
    }
    case EDecimalQuantization::None:
    {
      return RoundedVector;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return FVector{0};
}

FRotator UGenMovementComponent::RoundRotator(const FRotator& RotatorToRound, EDecimalQuantization Level)
{
  FRotator RoundedRotator = RotatorToRound;
  switch (Level)
  {
    case EDecimalQuantization::RoundTwoDecimals:
    {
      RoundedRotator.Roll = FMath::RoundToFloat(RotatorToRound.Roll * 100.f) / 100.f;
      RoundedRotator.Pitch = FMath::RoundToFloat(RotatorToRound.Pitch * 100.f) / 100.f;
      RoundedRotator.Yaw = FMath::RoundToFloat(RotatorToRound.Yaw * 100.f) / 100.f;
      return RoundedRotator;
    }
    case EDecimalQuantization::RoundOneDecimal:
    {
      RoundedRotator.Roll = FMath::RoundToFloat(RotatorToRound.Roll * 10.f) / 10.f;
      RoundedRotator.Pitch = FMath::RoundToFloat(RotatorToRound.Pitch * 10.f) / 10.f;
      RoundedRotator.Yaw = FMath::RoundToFloat(RotatorToRound.Yaw * 10.f) / 10.f;
      return RoundedRotator;
    }
    case EDecimalQuantization::RoundWholeNumber:
    {
      RoundedRotator.Roll = FMath::RoundToFloat(RotatorToRound.Roll);
      RoundedRotator.Pitch = FMath::RoundToFloat(RotatorToRound.Pitch);
      RoundedRotator.Yaw = FMath::RoundToFloat(RotatorToRound.Yaw);
      return RoundedRotator;
    }
    case EDecimalQuantization::None:
    {
      return RoundedRotator;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return FRotator{0};
}

bool UGenMovementComponent::IsMovable(UPrimitiveComponent* Component)
{
  return Component && Component->Mobility == EComponentMobility::Movable;
}

FVector UGenMovementComponent::GetLinearVelocity(UPrimitiveComponent* Component)
{
  FVector ComponentVelocity{0};
  if (IsMovable(Component))
  {
    // Check if the component has a velocity.
    ComponentVelocity = Component->GetComponentVelocity();
    if (!ComponentVelocity.IsZero()) return ComponentVelocity;

    // Check if the owner's root component has a velocity.
    if (const auto Owner = Component->GetOwner())
    {
      ComponentVelocity = Owner->GetVelocity();
      if (!ComponentVelocity.IsZero()) return ComponentVelocity;
    }

    // Check if the component has a physics velocity.
    if (const auto BodyInstance = Component->GetBodyInstance())
    {
      ComponentVelocity = BodyInstance->GetUnrealWorldVelocity();
    }
  }
  return ComponentVelocity;
}

FVector UGenMovementComponent::ComputeTangentialVelocity(const FVector& WorldLocation, UPrimitiveComponent* Component)
{
  if (IsMovable(Component))
  {
    if (const auto BodyInstance = Component->GetBodyInstance())
    {
      const FVector AngularVelocity = BodyInstance->GetUnrealWorldAngularVelocityInRadians();
      if (!AngularVelocity.IsNearlyZero())
      {
        const FVector Location = Component->GetComponentLocation();
        const FVector RadialDistanceToComponent = WorldLocation - Location;
        const FVector TangentialVelocity = AngularVelocity ^ RadialDistanceToComponent;
        return TangentialVelocity;
      }
    }
  }
  return FVector{0};
}

void UGenMovementComponent::ImpartVelocityFrom(
  UPrimitiveComponent* Source,
  bool bAddLinearVelocity,
  bool bAddAngularVelocity,
  float DeltaTime
)
{
  if (!Source || !bAddLinearVelocity && !bAddAngularVelocity)
  {
    return;
  }

  if (bAddLinearVelocity)
  {
    const FVector BaseLinearVelocity = GetLinearVelocity(Source);
    AddVelocity(BaseLinearVelocity, DeltaTime);
  }
  if (bAddAngularVelocity)
  {
    const FVector BaseTangentialVelocity = ComputeTangentialVelocity(GetLowerBound(), Source);
    AddVelocity(BaseTangentialVelocity, DeltaTime);
  }
}

FVector UGenMovementComponent::GetLowerBound() const
{
  const FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
  const FVector Bounds = UpdatedComponent->Bounds.BoxExtent;
  return FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - Bounds.Z);
}

FVector UGenMovementComponent::CalculateAirResistance(const FVector& CurrentVelocity, float DragCoefficient)
{
  if (DragCoefficient <= 0.f) return FVector{0};
  return -CurrentVelocity.GetSafeNormal() * CurrentVelocity.SizeSquared() * DragCoefficient;
}

FVector UGenMovementComponent::CalculateRollingResistance(
  const FVector& CurrentVelocity,
  float PawnMass,
  float GravityZ,
  float RollingCoefficient
)
{
  if (RollingCoefficient <= 0.f || PawnMass < KINDA_SMALL_NUMBER) return FVector{0};
  const float NormalForce = PawnMass * FMath::Abs(GravityZ);
  return -CurrentVelocity.GetSafeNormal() * NormalForce * RollingCoefficient;
}

FVector UGenMovementComponent::GetPlaneNormalWithWorldZ(const FVector& Direction)
{
  return (Direction ^ FVector::UpVector).GetSafeNormal();
}

bool UGenMovementComponent::UpdateFloor(FFloorParams& Floor, float TraceLength)
{
  SCOPE_CYCLE_COUNTER(STAT_UpdateFloor)

  FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
  bool bNoAdjustment{true};

  // Execute the shape trace.
  bool bValidShapeHitData{false};
  FHitResult ShapeHit = SweepRootCollisionSingleByChannel(FVector::DownVector, TraceLength);
  if (ShapeHit.bStartPenetrating)
  {
    // Move a little further away from the surface and try again. This should not be a major adjustement.
    FVector PenetrationAdjustment = GetPenetrationAdjustment(ShapeHit);
    constexpr float MaxSizePenetrationAdjustment = 2.f * UU_CENTIMETER;
    PenetrationAdjustment = PenetrationAdjustment.GetClampedToMaxSize(MaxSizePenetrationAdjustment);
    bNoAdjustment = !ResolvePenetration(PenetrationAdjustment, ShapeHit, UpdatedComponent->GetComponentQuat());
    if (!bNoAdjustment)
    {
      FLog(
        Verbose,
        "Updated component location was adjusted by {%s} after the initial shape trace started in penetration.",
        *(UpdatedComponent->GetComponentLocation() - CurrentLocation).ToString()
      )
      // The pawn's position was adjusted.
      CurrentLocation = UpdatedComponent->GetComponentLocation();
      ShapeHit = SweepRootCollisionSingleByChannel(FVector::DownVector, TraceLength);
    }
  }

  if (ShapeHit.IsValidBlockingHit())
  {
    // Only consider hits that are below the vertical portion of the collision shape.
    const float MaxSweepZ = CurrentLocation.Z - GetRootCollisionOuterHalfHeight();
    if (ShapeHit.ImpactPoint.Z < MaxSweepZ)
    {
      bValidShapeHitData = true;
      FLog(VeryVerbose, "Has valid shape hit (ShapeHit.Distance = %f).", ShapeHit.Distance)
    }
  }

  // Execute the line trace.
  bool bValidLineHitData{false};
  FHitResult LineHit;
  const FVector LineTraceStart = GetLowerBound();
  const FVector LineTraceEnd = LineTraceStart + FVector::DownVector * TraceLength;
  FCollisionQueryParams CollisionQueryParams(FName(__func__), false, GetOwner());
  if (const auto World = GetWorld())
  {
    World->LineTraceSingleByChannel(LineHit, LineTraceStart, LineTraceEnd, UpdatedComponent->GetCollisionObjectType(), CollisionQueryParams);
  }
  if (LineHit.IsValidBlockingHit())
  {
    bValidLineHitData = true;
    FLog(VeryVerbose, "Has valid line hit (LineHit.Distance = %f).", LineHit.Distance)
  }

  // Save the new floor.
  Floor = FFloorParams(ShapeHit, LineHit, CurrentLocation);

  DEBUG_OMC_SHOW_FLOOR_SWEEP
  // Return true if the shape trace did not start in penetration or the position was not adjusted (if it did start in penetration).
  return bNoAdjustment;
}

bool UGenMovementComponent::CanMove() const
{
  if (!UpdatedComponent || !PawnOwner) return false;
  return UpdatedComponent->Mobility == EComponentMobility::Movable
    && Mass >= KINDA_SMALL_NUMBER
    && !PawnOwner->IsMatineeControlled();
}

bool UGenMovementComponent::HasMoveInputEnabled() const
{
  if (const auto Owner = GetGenPawnOwner())
  {
    bool bInputEnabled = Owner->GetInputMode() != EInputMode::None && Owner->InputEnabled();
    if (const auto Controller = Owner->GetController())
    {
      bInputEnabled &= !Controller->IsMoveInputIgnored();
      if (const auto PlayerController = Cast<APlayerController>(Controller))
      {
        bInputEnabled &= PlayerController->InputEnabled();
      }
      return bInputEnabled;
    }
  }
  return false;
}

void UGenMovementComponent::HaltMovement()
{
  Velocity = Velocity0/*zero start velocity as well*/ = Acceleration = Force = FVector::ZeroVector;
}

EGenCollisionShape UGenMovementComponent::GetRootCollisionShape() const
{
  if (const auto Owner = GetGenPawnOwner())
  {
    const auto RootComponent = Owner->GetRootComponent();
    // @attention The horizontal capsule must be checked first because it inherits from UCapsuleComponent.
    if (Cast<UFlatCapsuleComponent>(RootComponent)) return EGenCollisionShape::HorizontalCapsule;
    if (Cast<UCapsuleComponent>(RootComponent)) return EGenCollisionShape::VerticalCapsule;
    if (Cast<UBoxComponent>(RootComponent)) return EGenCollisionShape::Box;
    if (Cast<USphereComponent>(RootComponent)) return EGenCollisionShape::Sphere;
    FLog(Warning, "Collision shape not supported.")
  }
  return EGenCollisionShape::Invalid;
}

bool UGenMovementComponent::HasVerticalCapsuleCollision() const
{
  return UGenMovementComponent::GetRootCollisionShape() == EGenCollisionShape::VerticalCapsule;
}

bool UGenMovementComponent::HasHorizontalCapsuleCollision() const
{
  return UGenMovementComponent::GetRootCollisionShape() == EGenCollisionShape::HorizontalCapsule;
}

bool UGenMovementComponent::HasBoxCollision() const
{
  return UGenMovementComponent::GetRootCollisionShape() == EGenCollisionShape::Box;
}

bool UGenMovementComponent::HasSphereCollision() const
{
  return UGenMovementComponent::GetRootCollisionShape() == EGenCollisionShape::Sphere;
}

FVector UGenMovementComponent::GetRootCollisionExtent() const
{
  if (!HasValidRootCollision()) return FVector{0};
  USceneComponent* RootComponent = PawnOwner->GetRootComponent();
  switch (UGenMovementComponent::GetRootCollisionShape())
  {
    case EGenCollisionShape::VerticalCapsule:
    {
      UCapsuleComponent* RootVCapsule = Cast<UCapsuleComponent>(RootComponent);
      return RootVCapsule->GetCollisionShape().GetExtent();
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      UFlatCapsuleComponent* RootHCapsule = Cast<UFlatCapsuleComponent>(RootComponent);
      return RootHCapsule->GetCollisionShape().GetExtent();
    }
    case EGenCollisionShape::Box:
    {
      UBoxComponent* RootBox = Cast<UBoxComponent>(RootComponent);
      return RootBox->GetCollisionShape().GetExtent();
    }
    case EGenCollisionShape::Sphere:
    {
      USphereComponent* RootSphere = Cast<USphereComponent>(RootComponent);
      return RootSphere->GetCollisionShape().GetExtent();
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return FVector{0};
}

float UGenMovementComponent::GetRootCollisionHalfHeight() const
{
  if (!HasValidRootCollision()) return 0.f;
  const EGenCollisionShape RootCollisionShape = UGenMovementComponent::GetRootCollisionShape();
  const FVector Extent = GetRootCollisionExtent();
  if (RootCollisionShape == EGenCollisionShape::HorizontalCapsule)
  {
    // For a horizontal capsule the height is the radius.
    return Extent.X;
  }
  return Extent.Z;
}

float UGenMovementComponent::GetRootCollisionOuterHalfHeight() const
{
  if (!HasValidRootCollision()) return 0.f;
  const FVector Extent = GetRootCollisionExtent();
  switch (UGenMovementComponent::GetRootCollisionShape())
  {
    case EGenCollisionShape::VerticalCapsule: return Extent.Z/*HalfHeight*/ - Extent.X/*Radius*/;
    case EGenCollisionShape::Box: return Extent.Z;
    case EGenCollisionShape::HorizontalCapsule:
    case EGenCollisionShape::Sphere: return 0.f;
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return 0.f;
}

float UGenMovementComponent::GetRootCollisionWidth(const FVector& Direction) const
{
  if (!HasValidRootCollision()) return 0.f;
  const FVector Extent = GetRootCollisionExtent();
  float Width{0.f};
  switch (UGenMovementComponent::GetRootCollisionShape())
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::Sphere:
      Width = Extent.X;
      break;
    case EGenCollisionShape::HorizontalCapsule:
    case EGenCollisionShape::Box:
    {
      const FVector NormalizedDirectionXY = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
      if (NormalizedDirectionXY.IsZero()) return 0.f;
      Width = ComputeDistanceToRootCollisionBoundaryXY(Direction);
      break;
    }
    default: checkNoEntryGMC();
  }
  return Width;
}

float UGenMovementComponent::GetMaxDistanceToRootCollisionBoundaryXY() const
{
  if (!HasValidRootCollision()) return 0.f;
  const FVector Extent = GetRootCollisionExtent();
  float MaxWidth{0.f};
  switch (UGenMovementComponent::GetRootCollisionShape())
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::Sphere:
      MaxWidth = Extent.X;
      break;
    case EGenCollisionShape::HorizontalCapsule:
      MaxWidth = Extent.Z;
      break;
    case EGenCollisionShape::Box:
      MaxWidth = FVector(Extent.X, Extent.Y, 0.f).Size();
      break;
    default: checkNoEntryGMC();
  }
  return MaxWidth;
}

float UGenMovementComponent::ComputeDistanceRootCollisionToImpactPercentXY(const FVector& ImpactPoint) const
{
  if (!HasValidRootCollision()) return 0.f;

  const FVector CollisionCenter = UpdatedComponent->GetComponentLocation();
  const FVector CenterToImpactXY = FVector(ImpactPoint.X, ImpactPoint.Y, 0.f) - FVector(CollisionCenter.X, CollisionCenter.Y, 0.f);
  const FVector ImpactDirectionXY = CenterToImpactXY.GetSafeNormal();
  if (ImpactDirectionXY.IsNearlyZero())
  {
    // The impact point is at the center of the collision.
    return 0.f;
  }

  const float DistanceToImpactXY = CenterToImpactXY.Size();
  const float DistanceToBoundaryXY = ComputeDistanceToRootCollisionBoundaryXY(ImpactDirectionXY);
  checkGMC(DistanceToBoundaryXY > 0.f)
  return DistanceToImpactXY / DistanceToBoundaryXY;
}

void UGenMovementComponent::SetRootCollisionExtent(const FVector& NewExtent, bool bUpdateOverlaps)
{
  SCOPE_CYCLE_COUNTER(STAT_SetRootCollisionExtent)

  if (!HasValidRootCollision()) return;
  const EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  const FVector ValidExtent = GetValidExtent(CollisionShape, NewExtent);
  USceneComponent* RootComponent = GetGenPawnOwner()->GetRootComponent();
  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    {
      UCapsuleComponent* RootVCapsule = Cast<UCapsuleComponent>(RootComponent);
      RootVCapsule->SetCapsuleSize(ValidExtent.X, ValidExtent.Z, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      UFlatCapsuleComponent* RootHCapsule = Cast<UFlatCapsuleComponent>(RootComponent);
      RootHCapsule->SetCapsuleSize(ValidExtent.X, ValidExtent.Z, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::Box:
    {
      UBoxComponent* RootBox = Cast<UBoxComponent>(RootComponent);
      RootBox->SetBoxExtent(ValidExtent, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::Sphere:
    {
      USphereComponent* RootSphere = Cast<USphereComponent>(RootComponent);
      RootSphere->SetSphereRadius(ValidExtent.X, bUpdateOverlaps);
      return;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
}

void UGenMovementComponent::SetRootCollisionExtentSafe(const FVector& NewExtent, float Tolerance, bool bUpdateOverlaps)
{
  if (!HasValidRootCollision()) return;
  check(UpdatedComponent)

  FHitResult TestHit;
  const EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  const FVector ValidExtent = GetValidExtent(CollisionShape, NewExtent);
  if (
    !IsValidPosition(
      CollisionShape,
      ValidExtent,
      UpdatedComponent->GetComponentLocation(),
      UpdatedComponent->GetComponentQuat(),
      TestHit,
      UpdatedComponent->GetCollisionObjectType(),
      Tolerance
    )
  )
  {
    return;
  }

  SetRootCollisionExtent(ValidExtent, bUpdateOverlaps);
}

void UGenMovementComponent::SetRootCollisionHalfHeight(float NewHalfHeight, bool bUpdateOverlaps)
{
  if (!HasValidRootCollision()) return;
  FVector Extent = GetRootCollisionExtent();
  switch (UGenMovementComponent::GetRootCollisionShape())
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::Box:
    {
      SetRootCollisionExtent({Extent.X, Extent.Y, NewHalfHeight}, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      SetRootCollisionExtent({NewHalfHeight, NewHalfHeight, Extent.Z}, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::Sphere:
    {
      SetRootCollisionExtent({NewHalfHeight, NewHalfHeight, NewHalfHeight}, bUpdateOverlaps);
      return;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
}

void UGenMovementComponent::SetRootCollisionHalfHeightSafe(float NewHalfHeight, float Tolerance, bool bUpdateOverlaps)
{
  if (!HasValidRootCollision()) return;
  FVector Extent = GetRootCollisionExtent();
  switch (UGenMovementComponent::GetRootCollisionShape())
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::Box:
    {
      SetRootCollisionExtentSafe({Extent.X, Extent.Y, NewHalfHeight}, Tolerance, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      SetRootCollisionExtentSafe({NewHalfHeight, NewHalfHeight, Extent.Z}, Tolerance, bUpdateOverlaps);
      return;
    }
    case EGenCollisionShape::Sphere:
    {
      SetRootCollisionExtentSafe({NewHalfHeight, NewHalfHeight, NewHalfHeight}, Tolerance, bUpdateOverlaps);
      return;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
}

void UGenMovementComponent::SetRootCollisionRotation(const FQuat& NewRotation)
{
  if (!HasValidRootCollision()) return;
  UpdatedComponent->SetWorldRotation(NewRotation);
}

void UGenMovementComponent::SetRootCollisionRotation(const FRotator& NewRotation)
{
  SetRootCollisionRotation(NewRotation.Quaternion());
}

FRotator UGenMovementComponent::GetRootCollisionRotation() const
{
  if (!HasValidRootCollision()) return FRotator{0};
  return UpdatedComponent->GetComponentRotation();
}

void UGenMovementComponent::SetRootCollisionRotationSafe(const FQuat& NewRotation, float Tolerance)
{
  if (!HasValidRootCollision()) return;

  const FQuat InitialRotation = UpdatedComponent->GetComponentQuat();
  if (InitialRotation.Equals(NewRotation, 0.01f))
  {
    return;
  }

  const EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  const FVector CollisionExtent = GetRootCollisionExtent();
  FHitResult TestHit;
  if (
    !IsValidPosition(
      CollisionShape,
      CollisionExtent,
      UpdatedComponent->GetComponentLocation(),
      NewRotation,
      TestHit,
      UpdatedComponent->GetCollisionObjectType(),
      Tolerance
    )
  )
  {
    return;
  }

  SetRootCollisionRotation(NewRotation);
}

void UGenMovementComponent::SetRootCollisionRotationSafe(const FRotator& NewRotation, float Tolerance)
{
  SetRootCollisionRotationSafe(NewRotation.Quaternion(), Tolerance);
}

FCollisionShape UGenMovementComponent::GetFrom(EGenCollisionShape CollisionShape, const FVector& Extent) const
{
  const FVector ValidExtent = GetValidExtent(CollisionShape, Extent);
  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::HorizontalCapsule: return FCollisionShape::MakeCapsule(ValidExtent);
    case EGenCollisionShape::Box: return FCollisionShape::MakeBox(ValidExtent);
    case EGenCollisionShape::Sphere: return FCollisionShape::MakeSphere(ValidExtent.X);
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return FCollisionShape();
}

FQuat UGenMovementComponent::AddGenCapsuleRotation(const FQuat& Rotation) const
{
  if (const auto GenCapsule = Cast<UGenCapsuleComponent>(UpdatedComponent))
  {
    return (Rotation.GetNormalized() * FQuat(GenCapsule->GetGenCapsuleRotation())).GetNormalized();
  }
  return Rotation;
}

FRotator UGenMovementComponent::AddGenCapsuleRotation(const FRotator& Rotation) const
{
  if (const auto GenCapsule = Cast<UGenCapsuleComponent>(UpdatedComponent))
  {
    return (Rotation + GenCapsule->GetGenCapsuleRotation()).GetNormalized();
  }
  return Rotation;
}

FVector UGenMovementComponent::GetValidExtent(EGenCollisionShape CollisionShape, const FVector& Extent) const
{
  if (CollisionShape >= EGenCollisionShape::Invalid)
  {
    checkGMC(false)
    return FVector{0};
  }

  FVector ValidExtent{0};
  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::HorizontalCapsule:
    {
      ValidExtent.X = ValidExtent.Y = FMath::Max3(KINDA_SMALL_NUMBER, Extent.X, Extent.Y);
      ValidExtent.Z = FMath::Max3(KINDA_SMALL_NUMBER, Extent.X, Extent.Z);
      break;
    }
    case EGenCollisionShape::Box:
    {
      ValidExtent.X = FMath::Max(KINDA_SMALL_NUMBER, Extent.X);
      ValidExtent.Y = FMath::Max(KINDA_SMALL_NUMBER, Extent.Y);
      ValidExtent.Z = FMath::Max(KINDA_SMALL_NUMBER, Extent.Z);
      break;
    }
    case EGenCollisionShape::Sphere:
    {
      ValidExtent.X = FMath::Max3(Extent.X, Extent.Y, Extent.Z);
      ValidExtent.X = ValidExtent.Y = ValidExtent.Z = FMath::Max(KINDA_SMALL_NUMBER, ValidExtent.X);
      break;
    }
    default: checkNoEntryGMC();
  }
  return ValidExtent;
}

bool UGenMovementComponent::IsValidExtent(EGenCollisionShape CollisionShape, const FVector& Extent) const
{
  if (CollisionShape >= EGenCollisionShape::Invalid)
  {
    checkGMC(false)
    return false;
  }

  // No negative, zero or nearly zero components.
  if (Extent.X <= KINDA_SMALL_NUMBER || Extent.Y <= KINDA_SMALL_NUMBER || Extent.Z <= KINDA_SMALL_NUMBER)
  {
    return false;
  }

  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::HorizontalCapsule:
    {
      // X and Y both refer to the radius.
      if (!FMath::IsNearlyEqual(Extent.X, Extent.Y, KINDA_SMALL_NUMBER))
      {
        return false;
      }
      // The half height cannot be smaller than the radius.
      if (Extent.Z < Extent.X)
      {
        return false;
      }
      // The passed vector is a valid extent for a capsule.
      return true;
    }
    case EGenCollisionShape::Box:
    {
      // A box has no further restrictions, the passed vector is a valid extent for a box.
      return true;
    }
    case EGenCollisionShape::Sphere:
    {
      // All components refer to the radius.
      if (!FMath::IsNearlyEqual(Extent.X, Extent.Y, KINDA_SMALL_NUMBER))
      {
        return false;
      }
      if (!FMath::IsNearlyEqual(Extent.X, Extent.Z, KINDA_SMALL_NUMBER))
      {
        return false;
      }
      // The passed vector is a valid extent for a sphere.
      return true;
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return false;
}

float UGenMovementComponent::ComputeDistanceToRootCollisionBoundaryXY(const FVector& Direction) const
{
  const FVector NormalizedDirectionXY = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
  if (NormalizedDirectionXY.IsZero() || !HasValidRootCollision()) return 0.f;
  const EGenCollisionShape RootCollision = UGenMovementComponent::GetRootCollisionShape();
  const FVector Extent = GetRootCollisionExtent();
  if (Extent.IsNearlyZero(KINDA_SMALL_NUMBER)) return 0.f;
  float DistanceToBoundaryXY{0.f};
  switch (RootCollision)
  {
    // For vertical capsules and spheres the offset is always the radius.
    case EGenCollisionShape::VerticalCapsule:
    case EGenCollisionShape::Sphere:
    {
      DistanceToBoundaryXY = Extent.X;
      break;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      const float Radius = Extent.X;
      const float XExtentNoCap = Extent.Z - Radius;
      const FRotator CapsuleRotation = AddGenCapsuleRotation(FRotator::ZeroRotator);
      const FRotator CapsuleRotationYaw = {0.f, CapsuleRotation.Yaw, 0.f};
      const float CosAlpha = FMath::Abs(CapsuleRotationYaw.RotateVector(UpdatedComponent->GetRightVector()) | NormalizedDirectionXY);
      const float Alpha = FMath::Acos(CosAlpha);
      const float Theta = FMath::Atan(XExtentNoCap / Radius);
      if (Theta <= Alpha && Alpha <= PI - Theta || PI + Theta <= Alpha && Alpha <= 2.f * PI - Theta)
      {
        // We need to consider the capsule-hemispheres.
        const float Beta = 0.5f * PI - Alpha;
        DistanceToBoundaryXY = FMath::Cos(Beta) * (
          XExtentNoCap + FMath::Sqrt(
            FMath::Square(Radius) + (Radius - XExtentNoCap) * (Radius + XExtentNoCap) * FMath::Square(FMath::Tan(Beta))
          )
        );
      }
      else
      {
        // We don't need to consider the capsule-hemispheres.
        DistanceToBoundaryXY = Radius / CosAlpha;
      }
      DistanceToBoundaryXY = FMath::Abs(DistanceToBoundaryXY);
      break;
    }
    case EGenCollisionShape::Box:
    {
      const float XExtent = Extent.X;
      const float YExtent = Extent.Y;
      const float CosAlpha = FMath::Abs(UpdatedComponent->GetRightVector() | NormalizedDirectionXY);
      const float CosBeta = FMath::Abs(UpdatedComponent->GetForwardVector() | NormalizedDirectionXY);
      const float Alpha = FMath::Acos(CosAlpha);
      const float Theta = FMath::Atan(XExtent / YExtent);
      if (Theta <= Alpha && Alpha <= PI - Theta || PI + Theta <= Alpha && Alpha <= 2.f * PI - Theta)
      {
        DistanceToBoundaryXY = XExtent / CosBeta;
      }
      else
      {
        DistanceToBoundaryXY = YExtent / CosAlpha;
      }
      DistanceToBoundaryXY = FMath::Abs(DistanceToBoundaryXY);
      break;
    }
    default: checkNoEntryGMC();
  }
  return DistanceToBoundaryXY;
}

void UGenMovementComponent::RotateYawTowardsDirection(const FVector& Direction, float Rate)
{
  if (!HasValidRootCollision() || Direction.IsZero()) return;
  const FVector TargetDirection = FVector::VectorPlaneProject(Direction, UpdatedComponent->GetUpVector()).GetSafeNormal();
  if (TargetDirection.IsZero()) return;
  // Round the current rotation to avoid deviations between server and client when using FMath::FixedTurn which can yield differently signed
  // results for inputs that are not exactly the same (e.g. if the rotation yaw on the client was 90.000023, it could be 89.999992 on the
  // server).
  const FRotator CurrentRotation = RoundRotator(UpdatedComponent->GetComponentRotation(), EDecimalQuantization::RoundTwoDecimals);
  const float TargetRotationYaw = FRotator::NormalizeAxis(TargetDirection.Rotation().Yaw);
  const float CurrentRotationYaw = FRotator::NormalizeAxis(CurrentRotation.Yaw);
  const float RotationDeltaYaw = [this, Rate] {
    return Rate > 0.f ? FRotator::NormalizeAxis(FMath::Min(Rate * GetPhysDeltaTime(), 360.f)) : 360.f;
  }();

  if (FMath::IsNearlyEqual(CurrentRotationYaw, TargetRotationYaw, 0.01f)) return;

  const float NewRotationYaw = FMath::FixedTurn(CurrentRotationYaw, TargetRotationYaw, RotationDeltaYaw);
  const FRotator RotationDelta = FRotator(0.f, NewRotationYaw - CurrentRotationYaw, 0.f);
  UpdatedComponent->AddLocalRotation(RotationDelta);
}

bool UGenMovementComponent::RotateYawTowardsDirectionSafe(const FVector& Direction, float Rate)
{
  if (!HasValidRootCollision() || Direction.IsZero()) return false;

  const auto CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  if (CollisionShape == EGenCollisionShape::VerticalCapsule || CollisionShape == EGenCollisionShape::Sphere)
  {
    // For symmetric collision shapes the rotation does not matter so there is no need to check for collisions.
    RotateYawTowardsDirection(Direction, Rate);
    return true;
  }

  const FVector TargetDirection = FVector::VectorPlaneProject(Direction, UpdatedComponent->GetUpVector()).GetSafeNormal();
  if (TargetDirection.IsZero()) return false;
  // Round the current rotation to avoid deviations between server and client when using FMath::FixedTurn which can yield differently signed
  // results for inputs that are not exactly the same (e.g. if the rotation yaw on the client was 90.000023, it could be 89.999992 on the
  // server).
  const FRotator CurrentRotation = RoundRotator(UpdatedComponent->GetComponentRotation(), EDecimalQuantization::RoundTwoDecimals);
  const float TargetRotationYaw = FRotator::NormalizeAxis(TargetDirection.Rotation().Yaw);
  const float CurrentRotationYaw = FRotator::NormalizeAxis(CurrentRotation.Yaw);
  const float RotationDeltaYaw = [Rate, this] {
    return Rate > 0.f ? FRotator::NormalizeAxis(FMath::Min(Rate * GetPhysDeltaTime(), 360.f)) : 360.f;
  }();

  if (FMath::IsNearlyEqual(CurrentRotationYaw, TargetRotationYaw, 0.01f)) return true;

  const float NewRotationYaw = FMath::FixedTurn(CurrentRotationYaw, TargetRotationYaw, RotationDeltaYaw);
  const FRotator RotationDelta = FRotator(0.f, NewRotationYaw - CurrentRotationYaw, 0.f);
  const FQuat ProposedRotation = (CurrentRotation + RotationDelta).Quaternion();

  FHitResult Hit;
  const FVector CollisionExtent = GetRootCollisionExtent();
  const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
  const FVector PreAdjustLocation = UpdatedComponent->GetComponentLocation();
  bool bIsValidRotation = IsValidPosition(
    CollisionShape,
    CollisionExtent,
    PreAdjustLocation,
    ProposedRotation,
    Hit,
    CollisionChannel
  );

  if (Hit.bBlockingHit)
  {
    FScopedMovementUpdate ScopedMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);

    const FVector ForwardVector = UpdatedComponent->GetForwardVector();
    const float CollisionMaxWidth = GetMaxDistanceToRootCollisionBoundaryXY();
    const FVector RotationStart = CollisionMaxWidth * ForwardVector;
    const FVector RotationEnd = RotationDelta.RotateVector(RotationStart);
    float AdjustDistance = FMath::Min((RotationStart - RotationEnd).Size(), CollisionMaxWidth) + UU_MILLIMETER;
    // We only adjust in the XY-plane.
    const FVector ImpactNormalXY = Hit.ImpactNormal.GetSafeNormal2D();
    FVector AdjustDirection{0};
    if (!ImpactNormalXY.IsZero())
    {
      AdjustDirection = ImpactNormalXY;
      const float IntraShapeDistance = ComputeDistanceToRootCollisionBoundaryXY(-AdjustDirection);
      if (AdjustDistance > IntraShapeDistance)
      {
        // We never need to move further away from the obstruction than our collision shape is wide in the direction opposing adjustment.
        AdjustDistance = (AdjustDistance - IntraShapeDistance) + UU_MILLIMETER;
      }
    }
    else
    {
      // Sometimes we get bad impact normals (especially with box collisions) which can make the 2D impact normal a zero vector. In that
      // case we have to calculate the adjustment direction in another (worse) way.
      AdjustDirection = (UpdatedComponent->GetComponentLocation() - Hit.ImpactPoint).GetSafeNormal2D();
    }
    const FVector Adjustment = AdjustDirection * AdjustDistance;
    if (Adjustment.IsNearlyZero())
    {
      return false;
    }

    MoveUpdatedComponent(Adjustment, CurrentRotation, true, &Hit);
    if (Hit.bStartPenetrating)
    {
      ScopedMovement.RevertMove();
      return false;
    }

    bIsValidRotation = IsValidPosition(
      CollisionShape,
      CollisionExtent,
      UpdatedComponent->GetComponentLocation(),
      ProposedRotation,
      Hit,
      CollisionChannel
    );

    if (!bIsValidRotation)
    {
      ScopedMovement.RevertMove();
      return false;
    }
  }

  if (bIsValidRotation)
  {
    UpdatedComponent->SetWorldRotation(ProposedRotation);
    return true;
  }
  return false;
}

bool UGenMovementComponent::HasValidRootCollision() const
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
  const auto Owner = GetGenPawnOwner();
  if (!Owner) return false;

  UShapeComponent* RootComponent = Cast<UShapeComponent>(Owner->GetRootComponent());
  if (!RootComponent)
  {
    FLog(Warning, "Root collision invalid, the root component has to be of type UShapeComponent.")
    return false;
  }

  if (RootComponent != UpdatedComponent)
  {
    FLog(Warning, "Root collision invalid, the root component must be the updated component.")
    return false;
  }

  const FVector Scale = RootComponent->GetComponentScale();
  if (Scale != FVector::OneVector)
  {
    FLog(Warning, "Root collision invalid, scaling of the root component is not supported.")
    return false;
  }

  const EGenCollisionShape EShape = UGenMovementComponent::GetRootCollisionShape();
  if (EShape < EGenCollisionShape::Invalid)
  {
    const FCollisionShape FShape = RootComponent->GetCollisionShape();
    if (!IsValidExtent(EShape, FShape.GetExtent()))
    {
      FLog(Warning, "Root collision invalid, the current shape extent is not a valid extent vector.")
      return false;
    }
    // The root collision is valid.
    return true;
  }

  FLog(Warning, "Root collision invalid, the root component has an unsupported shape.")
  return false;
#else
  return true;
#endif
}

float UGenMovementComponent::ComputeImmersionDepth() const
{
  SCOPE_CYCLE_COUNTER(STAT_ComputeImmersionDepth)

  if (!HasValidRootCollision()) return 0.f;
  float ImmersionDepth = 0.f;
  if (const auto PhysicsVolume = GetPhysicsVolume())
  {
    if (PhysicsVolume->bWaterVolume)
    {
      float HalfHeightZ = GetRootCollisionHalfHeight();
      FVector CollisionHalfHeight = FVector(0.f, 0.f, HalfHeightZ);
      UBrushComponent* VolumeBrush = PhysicsVolume->GetBrushComponent();
      FHitResult HitResult;
      if (VolumeBrush)
      {
        FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
        FVector TraceStart = CurrentLocation + CollisionHalfHeight;
        FVector TraceEnd = CurrentLocation - CollisionHalfHeight;
        FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(ImmersionDepth), true);
        // Trace from the top of our root collision to its bottom against the physics volume's brush component.
        VolumeBrush->LineTraceComponent(HitResult, TraceStart, TraceEnd, CollisionQueryParams);
      }
      // The hit result time tells us how deep we are immersed.
      ImmersionDepth = 1.f - HitResult.Time;
    }
  }
  // Immersion depth will always be zero when not in a fluid volume.
  return ImmersionDepth;
}

bool UGenMovementComponent::CanStepUp(const FHitResult& Hit) const
{
  if (!Hit.IsValidBlockingHit())
  {
    return false;
  }
  UPrimitiveComponent* HitComponent = Hit.Component.Get();
  if (!HitComponent)
  {
    return false;
  }
  // @see UPrimitiveComponent::CanCharacterStepUp will also call @see AActor::CanBeBaseForCharacter if needed so a separate check is not
  // necessary.
  if (!HitComponent->CanCharacterStepUp(PawnOwner))
  {
    return false;
  }
  // @see UPrimitiveComponent::CanCharacterStepUp will already call @see AActor::CanBeBaseForCharacter on the owner of the hit component if
  // appropriate, but it may be possible that the hit actor is not the owner of the hit component (?).
  AActor* HitActor = Hit.GetActor();
  // The hit actor can be null (e.g. BSPs).
  if (HitActor)
  {
    if (!HitActor->CanBeBaseForCharacter(PawnOwner))
    {
      return false;
    }
  }
  return true;
}

FHitResult UGenMovementComponent::K2_SweepRootCollisionSingleByChannel_Direction(
  const FVector& Direction,
  float TraceLength,
  const FVector& Extent,
  const FRotator& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  return SweepRootCollisionSingleByChannel(Direction, TraceLength, Extent, Rotation.Quaternion(), CollisionChannel);
}

FHitResult UGenMovementComponent::SweepRootCollisionSingleByChannel(
  const FVector& Direction,
  float TraceLength,
  const FVector& Extent,
  const FQuat& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  UWorld* World = GetWorld();
  const FVector NormalizedDirection = Direction.GetSafeNormal();
  if (!World || NormalizedDirection.IsZero() || TraceLength <= 0.f || !HasValidRootCollision()) return FHitResult();

  const FTransform& Transform = UpdatedComponent->GetComponentTransform();
  const FVector TraceStart = Transform.GetLocation();
  const FVector TraceEnd = TraceStart + NormalizedDirection * TraceLength;
  return SweepRootCollisionSingleByChannel(TraceStart, TraceEnd, Extent, Rotation, CollisionChannel);
}

FHitResult UGenMovementComponent::K2_SweepRootCollisionSingleByChannel(
  const FVector& TraceStart,
  const FVector& TraceEnd,
  const FVector& Extent,
  const FRotator& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  return SweepRootCollisionSingleByChannel(TraceStart, TraceEnd, Extent, Rotation.Quaternion(), CollisionChannel);
}

FHitResult UGenMovementComponent::SweepRootCollisionSingleByChannel(
  const FVector& TraceStart,
  const FVector& TraceEnd,
  const FVector& Extent,
  const FQuat& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  UWorld* World = GetWorld();
  if (!World || TraceStart == TraceEnd || !HasValidRootCollision()) return FHitResult();

  FHitResult HitResult;
  const EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  const FCollisionShape TraceShape =
    GetFrom(CollisionShape, Extent.IsZero() ? GetRootCollisionExtent() : GetValidExtent(CollisionShape, Extent));
  const FTransform& Transform = UpdatedComponent->GetComponentTransform();
  FQuat TraceRotation = (Rotation * AddGenCapsuleRotation(UpdatedComponent->GetComponentQuat())).GetNormalized();
  TraceRotation.Normalize();
  FCollisionQueryParams CollisionQueryParams(FName(__func__), false, GetOwner());
  World->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, TraceRotation, CollisionChannel, TraceShape, CollisionQueryParams);
  return HitResult;
}

TArray<FHitResult> UGenMovementComponent::K2_SweepRootCollisionMultiByChannel_Direction(
  const FVector& Direction,
  float TraceLength,
  const FVector& Extent,
  const FRotator& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  return SweepRootCollisionMultiByChannel(Direction, TraceLength, Extent, Rotation.Quaternion(), CollisionChannel);
}

TArray<FHitResult> UGenMovementComponent::SweepRootCollisionMultiByChannel(
  const FVector& Direction,
  float TraceLength,
  const FVector& Extent,
  const FQuat& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  UWorld* World = GetWorld();
  const FVector NormalizedDirection = Direction.GetSafeNormal();
  if (!World || NormalizedDirection.IsZero() || TraceLength <= 0.f || !HasValidRootCollision()) return TArray<FHitResult>();

  const FTransform& Transform = UpdatedComponent->GetComponentTransform();
  const FVector TraceStart = Transform.GetLocation();
  const FVector TraceEnd = TraceStart + NormalizedDirection * TraceLength;
  return SweepRootCollisionMultiByChannel(TraceStart, TraceEnd, Extent, Rotation, CollisionChannel);
}

TArray<FHitResult> UGenMovementComponent::K2_SweepRootCollisionMultiByChannel(
  const FVector& TraceStart,
  const FVector& TraceEnd,
  const FVector& Extent,
  const FRotator& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  return SweepRootCollisionMultiByChannel(TraceStart, TraceEnd, Extent, Rotation.Quaternion(), CollisionChannel);
}

TArray<FHitResult> UGenMovementComponent::SweepRootCollisionMultiByChannel(
  const FVector& TraceStart,
  const FVector& TraceEnd,
  const FVector& Extent,
  const FQuat& Rotation,
  ECollisionChannel CollisionChannel
) const
{
  UWorld* World = GetWorld();
  TArray<FHitResult> HitResults;
  if (!World || TraceStart == TraceEnd || !HasValidRootCollision()) return HitResults;

  const EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  const FCollisionShape TraceShape =
    GetFrom(CollisionShape, Extent.IsZero() ? GetRootCollisionExtent() : GetValidExtent(CollisionShape, Extent));
  const FTransform& Transform = UpdatedComponent->GetComponentTransform();
  FQuat TraceRotation = (Rotation * AddGenCapsuleRotation(UpdatedComponent->GetComponentQuat())).GetNormalized();
  const FCollisionQueryParams CollisionQueryParams(FName(__func__), false, GetOwner());
  World->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, TraceRotation, CollisionChannel, TraceShape, CollisionQueryParams);
  return HitResults;
}

TArray<FHitResult> UGenMovementComponent::LineTracesAlignedVertical(
  const FVector& Direction,
  float TraceLength,
  float MaxHeight,
  int32 Resolution,
  ECollisionChannel CollisionChannel
) const
{
  UWorld* World = GetWorld();
  const FVector NormalizedDirection = Direction.GetSafeNormal();
  TArray<FHitResult> HitResults;
  if (!World || TraceLength <= 0.f || NormalizedDirection.IsZero() || MaxHeight <= 0.f || Resolution <= 0) return HitResults;

  // Calculate the parameters.
  const FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
  const FVector Bounds = UpdatedComponent->Bounds.BoxExtent;
  const FVector LowerBound = FVector(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z - Bounds.Z);
  const FVector TracesMaxHeight = LowerBound + FVector(0.f, 0.f, MaxHeight);
  const float TracesInterval = (TracesMaxHeight - LowerBound).Size() / Resolution;

  // Execute the line traces.
  FHitResult HitResult;
  FVector TraceStart = LowerBound;
  FVector TraceEnd = LowerBound + NormalizedDirection * TraceLength;
  FCollisionQueryParams CollisionQueryParams(FName(__func__), false, GetOwner());
  for (int NumTrace = 0; NumTrace < Resolution; ++NumTrace)
  {
    World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, CollisionQueryParams);
    HitResults.Emplace(HitResult);
    TraceStart += FVector(0.f, 0.f, TracesInterval);
    TraceEnd += FVector(0.f, 0.f, TracesInterval);
  }
  return HitResults;
}

FHitResult UGenMovementComponent::AutoResolvePenetration()
{
  FHitResult Hit;
  const FQuat CurrentRotation = UpdatedComponent->GetComponentQuat();
  SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), CurrentRotation, true, Hit);
  SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), CurrentRotation, true, Hit);
  return Hit;
}

bool UGenMovementComponent::IsWithinEdgeTolerance(const FVector& Location, const FVector& Point, float Tolerance) const
{
  const FVector LocationToPoint = Point - Location;
  const float CenterToEdgeXY = ComputeDistanceToRootCollisionBoundaryXY(LocationToPoint);
  const float DistanceFromCenterSq = LocationToPoint.SizeSquared2D();
  const float ReducedWidthSq = FMath::Square(FMath::Max(CenterToEdgeXY - Tolerance, Tolerance + KINDA_SMALL_NUMBER));
  const bool bIsWithinEdgeTolerance = DistanceFromCenterSq < ReducedWidthSq;
  FLog(VeryVerbose, "Test point %s within edge tolerance.", bIsWithinEdgeTolerance ? TEXT("is") : TEXT("not"))
  return bIsWithinEdgeTolerance;
}

float UGenMovementComponent::LerpRootCollisionHalfHeight(
  float TargetHalfHeight,
  float InterpSpeed,
  float InterpTolerance,
  float DeltaTime,
  bool bAdjustPosition,
  EAdjustDirection AdjustDirection
)
{
  if (!HasValidRootCollision()) return 0.f;
  TargetHalfHeight = FMath::Clamp(TargetHalfHeight, 2.f * KINDA_SMALL_NUMBER, BIG_NUMBER);
  DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
  if (DeltaTime < MIN_DELTA_TIME) return 0.f;

  EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  FVector Extent = GetRootCollisionExtent();
  const float InitialHalfHeight = CollisionShape == EGenCollisionShape::HorizontalCapsule ? Extent.X : Extent.Z;
  if (FMath::IsNearlyEqual(InitialHalfHeight, TargetHalfHeight, 0.000001f)) return 0.f;
  float NewHalfHeight = FMath::FInterpConstantTo(InitialHalfHeight, TargetHalfHeight, DeltaTime, InterpSpeed);

  // Clamp the new half height to a valid extent value.
  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    {
      NewHalfHeight = FMath::Max(Extent.X, NewHalfHeight);
      break;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      NewHalfHeight = FMath::Min(Extent.Z, NewHalfHeight);
      break;
    }
    case EGenCollisionShape::Box:
    case EGenCollisionShape::Sphere:
    {
      break;
    }
    default: checkNoEntryGMC() return 0.f;
  }
  if (FMath::IsNearlyEqual(NewHalfHeight, InitialHalfHeight, 0.000001f)) return 0.f;

  const auto SetNewExtent = [&]() {
    switch (CollisionShape)
    {
      case EGenCollisionShape::VerticalCapsule:
      case EGenCollisionShape::Box:
      {
        SetRootCollisionExtentSafe({Extent.X, Extent.Y, NewHalfHeight}, InterpTolerance);
        return FMath::Abs(GetRootCollisionExtent().Z - InitialHalfHeight);
      }
      case EGenCollisionShape::HorizontalCapsule:
      {
        SetRootCollisionExtentSafe({NewHalfHeight, NewHalfHeight, Extent.Z}, InterpTolerance);
        return FMath::Abs(GetRootCollisionExtent().X - InitialHalfHeight);
      }
      case EGenCollisionShape::Sphere:
      {
        SetRootCollisionExtentSafe({NewHalfHeight, NewHalfHeight, NewHalfHeight}, InterpTolerance);
        return FMath::Abs(GetRootCollisionExtent().X - InitialHalfHeight);
      }
      default: checkNoEntryGMC();
    }
    checkNoEntryGMC()
    return 0.f;
  };

  // If we are not going to move the root collision just set the new extent.
  if (!bAdjustPosition)
  {
    return SetNewExtent();
  }

  const auto AdjustPosition = [this](float ZAdjustment) {
    FHitResult Hit;
    SafeMoveUpdatedComponent({0.f, 0.f, ZAdjustment}, UpdatedComponent->GetComponentQuat(), true, Hit);
    return Hit;
  };

  // If the collision is going to get larger we should move first, then set the new extent. If the collision is going to get smaller we want
  // to do it the other way round.
  const float Adjustment = FMath::Abs(NewHalfHeight - InitialHalfHeight) * (AdjustDirection == EAdjustDirection::Up ? 1.f : -1.f);
  if (NewHalfHeight > InitialHalfHeight)
  {
    FScopedMovementUpdate ScopedMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);

    if (AdjustPosition(Adjustment).Time < 1.f)
    {
      // The pawn was blocked while adjusting the position.
      ScopedMovement.RevertMove();
      return 0.f;
    }

    const float HalfHeightChange = SetNewExtent();
    if (HalfHeightChange == 0.f)
    {
      // The new extent would cause a blocking collision.
      ScopedMovement.RevertMove();
      return 0.f;
    }

    return HalfHeightChange;
  }

  const float HalfHeightChange = SetNewExtent();
  if (HalfHeightChange == 0.f)
  {
    // The new extent would cause a blocking collision.
    return 0.f;
  }

  // Since the collision extent got smaller and we don't move the pawn by more than the half height was changed a blocking collision should
  // never occur in this case.
  AdjustPosition(Adjustment);

  return HalfHeightChange;
}

float UGenMovementComponent::LerpRootCollisionWidth(
  float TargetWidth,
  float InterpSpeed,
  float InterpTolerance,
  float DeltaTime,
  bool bInterpBoxY
)
{
  if (!HasValidRootCollision()) return 0.f;
  TargetWidth = FMath::Clamp(TargetWidth, 2.f * KINDA_SMALL_NUMBER, BIG_NUMBER);
  DeltaTime = DeltaTime >= MIN_DELTA_TIME ? DeltaTime : GetPhysDeltaTime();
  if (DeltaTime < MIN_DELTA_TIME) return 0.f;

  EGenCollisionShape CollisionShape = UGenMovementComponent::GetRootCollisionShape();
  FVector Extent = GetRootCollisionExtent();
  bInterpBoxY = CollisionShape == EGenCollisionShape::Box ? bInterpBoxY : false;
  const float InitialWidth = CollisionShape == EGenCollisionShape::HorizontalCapsule ? Extent.Z : bInterpBoxY ? Extent.Y : Extent.X;
  if (FMath::IsNearlyEqual(InitialWidth, TargetWidth, 0.000001f)) return 0.f;
  float NewWidth = FMath::FInterpConstantTo(InitialWidth, TargetWidth, DeltaTime, InterpSpeed);

  // Clamp the new width to a valid extent value.
  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    {
      NewWidth = FMath::Min(Extent.Z, NewWidth);
      break;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      NewWidth = FMath::Max(Extent.X, NewWidth);
      break;
    }
    case EGenCollisionShape::Box:
    case EGenCollisionShape::Sphere:
    {
      break;
    }
    default: checkNoEntryGMC() return 0.f;
  }
  if (FMath::IsNearlyEqual(NewWidth, InitialWidth, 0.000001f)) return 0.f;

  switch (CollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    {
      SetRootCollisionExtentSafe({NewWidth, NewWidth, Extent.Z}, InterpTolerance);
      return FMath::Abs(GetRootCollisionExtent().X - InitialWidth);
    }
    case EGenCollisionShape::Box:
    {
      if (bInterpBoxY)
      {
        SetRootCollisionExtentSafe({Extent.X, NewWidth, Extent.Z}, InterpTolerance);
        return FMath::Abs(GetRootCollisionExtent().Y - InitialWidth);
      }
      SetRootCollisionExtentSafe({NewWidth, Extent.Y, Extent.Z}, InterpTolerance);
      return FMath::Abs(GetRootCollisionExtent().X - InitialWidth);
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      SetRootCollisionExtentSafe({Extent.X, Extent.Y, NewWidth}, InterpTolerance);
      return FMath::Abs(GetRootCollisionExtent().Z - InitialWidth);
    }
    case EGenCollisionShape::Sphere:
    {
      SetRootCollisionExtentSafe({NewWidth, NewWidth, NewWidth}, InterpTolerance);
      return FMath::Abs(GetRootCollisionExtent().X - InitialWidth);
    }
    default: checkNoEntryGMC();
  }
  checkNoEntryGMC()
  return 0.f;
}

USceneComponent* UGenMovementComponent::SetRootCollisionShape(EGenCollisionShape NewCollisionShape, const FVector& Extent, FName Name)
{
  SCOPE_CYCLE_COUNTER(STAT_SetRootCollisionShape)

  checkGMC(PawnOwner)
  const auto OriginalRootComponent = PawnOwner->GetRootComponent();
  checkGMC(OriginalRootComponent)
  if (NewCollisionShape >= EGenCollisionShape::Invalid)
  {
    checkGMC(false)
    return OriginalRootComponent;
  }

  const FVector ValidExtent = GetValidExtent(NewCollisionShape, Extent);
  if (NewCollisionShape == UGenMovementComponent::GetRootCollisionShape())
  {
    // The root component already has the requested collision shape. Only update the extent and the name.
    SetRootCollisionExtent(ValidExtent);
    OriginalRootComponent->Rename(*Name.ToString());
    return OriginalRootComponent;
  }

  USceneComponent* NewRootComponent{nullptr};
  switch (NewCollisionShape)
  {
    case EGenCollisionShape::VerticalCapsule:
    {
      NewRootComponent = NewObject<UCapsuleComponent>(PawnOwner, UCapsuleComponent::StaticClass(), Name, RF_Transactional);
      break;
    }
    case EGenCollisionShape::HorizontalCapsule:
    {
      NewRootComponent = NewObject<UFlatCapsuleComponent>(PawnOwner, UFlatCapsuleComponent::StaticClass(), Name, RF_Transactional);
      break;
    }
    case EGenCollisionShape::Box:
    {
      NewRootComponent = NewObject<UBoxComponent>(PawnOwner, UBoxComponent::StaticClass(), Name, RF_Transactional);
      break;
    }
    case EGenCollisionShape::Sphere:
    {
      NewRootComponent = NewObject<USphereComponent>(PawnOwner, USphereComponent::StaticClass(), Name, RF_Transactional);
      break;
    }
    default: checkNoEntryGMC() return OriginalRootComponent;
  }

  TArray<USceneComponent*> RootChildrenExclusive;
  OriginalRootComponent->GetChildrenComponents(false, RootChildrenExclusive);
  PawnOwner->AddInstanceComponent(NewRootComponent);
  NewRootComponent->RegisterComponent();
  PawnOwner->Modify();
  NewRootComponent->AttachToComponent(OriginalRootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
  for (USceneComponent* Child : RootChildrenExclusive)
  {
    Child->AttachToComponent(NewRootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
  }
  const auto OriginalRootComponentShape = Cast<UShapeComponent>(OriginalRootComponent);
  const auto NewRootComponentShape = Cast<UShapeComponent>(NewRootComponent);
  // The original root component may not have been a UShapeComponent. In that case we don't copy any settings and just use the defaults.
  if (OriginalRootComponentShape && NewRootComponentShape)
  {
    CopyComponentSettings(OriginalRootComponentShape, NewRootComponentShape);
  }
  PawnOwner->SetRootComponent(NewRootComponent);
  SetUpdatedComponent(NewRootComponent);
  checkGMC(UpdatedComponent && UpdatedPrimitive)
  SetRootCollisionExtent(ValidExtent);
  OriginalRootComponent->UnregisterComponent();
  OriginalRootComponent->DestroyComponent(false);
  return NewRootComponent;
}

USceneComponent* UGenMovementComponent::SetRootCollisionShapeSafe(
  EGenCollisionShape NewCollisionShape,
  const FVector& Extent,
  FName Name,
  float Tolerance
)
{
  checkGMC(PawnOwner)
  const auto OriginalRootComponent = PawnOwner->GetRootComponent();
  checkGMC(OriginalRootComponent)
  if (NewCollisionShape >= EGenCollisionShape::Invalid)
  {
    checkGMC(false)
    return OriginalRootComponent;
  }
  check(UpdatedComponent)

  FHitResult TestHit;
  const FVector ValidExtent = GetValidExtent(NewCollisionShape, Extent);
  if (
    !IsValidPosition(
      NewCollisionShape,
      ValidExtent,
      UpdatedComponent->GetComponentLocation(),
      UpdatedComponent->GetComponentQuat(),
      TestHit,
      UpdatedComponent->GetCollisionObjectType(),
      Tolerance
    )
  )
  {
    return OriginalRootComponent;
  }

  return SetRootCollisionShape(NewCollisionShape, ValidExtent, Name);
}

bool UGenMovementComponent::IsValidPosition(
  EGenCollisionShape CollisionShape,
  const FVector& Extent,
  const FVector& Location,
  const FQuat& Rotation,
  FHitResult& OutHit,
  ECollisionChannel CollisionChannel,
  float Tolerance
) const
{
  SCOPE_CYCLE_COUNTER(STAT_IsValidPosition)

  if (CollisionShape >= EGenCollisionShape::Invalid)
  {
    checkGMC(false)
    return false;
  }

  if (!IsValidExtent(CollisionShape, Extent))
  {
    return false;
  }

  if (Tolerance <= 0.f)
  {
    // Logically a tolerance of 0 means we can consider any position valid.
    return true;
  }

  UWorld* World = GetWorld();
  if (!World) return false;

  const FVector ScaledExtent = Extent * Tolerance;
  FCollisionShape TraceShape = GetFrom(CollisionShape, ScaledExtent);
  FQuat TraceRotation = AddGenCapsuleRotation(Rotation);
  FCollisionQueryParams CollisionQueryParams(FName(__func__), false, GetOwner());

  const auto IsBlocked = [&](const FVector& Offset) {
    World->SweepSingleByChannel(
      OutHit,
      Location,
      Location + Offset,
      TraceRotation,
      CollisionChannel,
      TraceShape,
      CollisionQueryParams
    );

    if (OutHit.bBlockingHit)
    {
      // Not a valid location for the test shape.
      return true;
    }
    checkGMC(!OutHit.bStartPenetrating)
    return false;
  };

  if (IsBlocked({0.f, 0.f, 0.01f}))
  {
    return false;
  }

  if (IsBlocked({0.f, 0.f, -0.01f}))
  {
    return false;
  }

  return true;
}

bool UGenMovementComponent::IsValidPosition(
  EGenCollisionShape CollisionShape,
  const FVector& Extent,
  const FVector& Location,
  const FRotator& Rotation,
  FHitResult& OutHit,
  ECollisionChannel CollisionChannel,
  float Tolerance
) const
{
  return IsValidPosition(CollisionShape, Extent, Location, Rotation.Quaternion(), OutHit, CollisionChannel, Tolerance);
}

void UGenMovementComponent::CopyComponentSettings(UShapeComponent* Source, UShapeComponent* Target)
{
  if (!Source || !Target) return;
  const auto SourceBI = Source->GetBodyInstance(NAME_None, false);
  const auto TargetBI = Target->GetBodyInstance(NAME_None, false);

  // Transform.
  Target->SetWorldScale3D(Source->GetComponentScale());
  Target->SetMobility(Source->Mobility);

  // Shape.
  Target->ShapeColor = Source->ShapeColor;

  // Navigation.
  Target->AreaClass = Source->AreaClass;
  Target->bDynamicObstacle = Source->bDynamicObstacle;
  Target->bFillCollisionUnderneathForNavmesh = Source->bFillCollisionUnderneathForNavmesh;

  // Component Tick.
  Target->SetComponentTickEnabled(Source->IsComponentTickEnabled());
  Target->PrimaryComponentTick.bStartWithTickEnabled = Source->PrimaryComponentTick.bStartWithTickEnabled;
  Target->SetComponentTickInterval(Source->GetComponentTickInterval());
  Target->SetTickableWhenPaused(Source->PrimaryComponentTick.bTickEvenWhenPaused);
  Target->PrimaryComponentTick.bAllowTickOnDedicatedServer = Source->PrimaryComponentTick.bAllowTickOnDedicatedServer;
  Target->SetTickGroup(Source->PrimaryComponentTick.TickGroup);

  // Rendering.
  Target->SetVisibility(Source->GetVisibleFlag(), false);
  Target->SetHiddenInGame(Source->bHiddenInGame);
  Target->bVisibleInReflectionCaptures = Source->bVisibleInReflectionCaptures;
#if !UE_VERSION_OLDER_THAN(4, 26, 0)
  Target->bVisibleInRealTimeSkyCaptures = Source->bVisibleInRealTimeSkyCaptures;
#endif
  Target->bVisibleInRayTracing = Source->bVisibleInRayTracing;
  Target->SetRenderInMainPass(Source->bRenderInMainPass);
  Target->bRenderInDepthPass = Source->bRenderInDepthPass;
  Target->SetReceivesDecals(Source->bReceivesDecals);
  Target->SetOwnerNoSee(Source->bOwnerNoSee);
  Target->SetOnlyOwnerSee(Source->bOnlyOwnerSee);
  Target->bTreatAsBackgroundForOcclusion = Source->bTreatAsBackgroundForOcclusion;
  Target->bUseAsOccluder = Source->bUseAsOccluder;
  Target->SetRenderCustomDepth(Source->bRenderCustomDepth);
  Target->SetCustomDepthStencilWriteMask(Source->CustomDepthStencilWriteMask);
  Target->SetCustomDepthStencilValue(Source->CustomDepthStencilValue);
  //Target->CustomPrimitiveDataInternal = Source->GetCustomPrimitiveData();
  Target->SetTranslucentSortPriority(Source->TranslucencySortPriority);
#if UE_VERSION_OLDER_THAN(5, 0, 0)
  Target->LpvBiasMultiplier = Source->LpvBiasMultiplier;
#endif
  Target->BoundsScale = Source->BoundsScale;
  Target->bUseAttachParentBound = Source->bUseAttachParentBound;

  // Physics.
  Target->SetSimulatePhysics(Source->IsSimulatingPhysics());
  if (SourceBI && TargetBI && SourceBI->bOverrideMass)
  {
    Target->SetMassOverrideInKg(NAME_None, SourceBI->GetMassOverride(), true);
  }
  Target->SetLinearDamping(Source->GetLinearDamping());
  Target->SetAngularDamping(Source->GetAngularDamping());
  Target->SetEnableGravity(Source->IsGravityEnabled());
  if (SourceBI && TargetBI)
  {
    TargetBI->bLockRotation = SourceBI->bLockRotation;
    TargetBI->bLockXRotation = SourceBI->bLockXRotation;
    TargetBI->bLockYRotation = SourceBI->bLockYRotation;
    TargetBI->bLockZRotation = SourceBI->bLockZRotation;
    TargetBI->bLockRotation = SourceBI->bLockRotation;
    TargetBI->bLockXRotation = SourceBI->bLockXRotation;
    TargetBI->bLockYRotation = SourceBI->bLockYRotation;
    TargetBI->bLockZRotation = SourceBI->bLockZRotation;
    Target->SetConstraintMode(SourceBI->DOFMode);
  }
  Target->bIgnoreRadialImpulse = Source->bIgnoreRadialImpulse;
  Target->bIgnoreRadialForce = Source->bIgnoreRadialForce;
  Target->bApplyImpulseOnDamage = Source->bApplyImpulseOnDamage;
  if (SourceBI && TargetBI)
  {
    TargetBI->bAutoWeld = SourceBI->bAutoWeld;
    TargetBI->bStartAwake = SourceBI->bStartAwake;
    TargetBI->COMNudge = SourceBI->COMNudge;
  }
  Target->SetMassScale(NAME_None, Source->GetMassScale());
  if (SourceBI && TargetBI)
  {
    if (SourceBI->bOverrideMaxAngularVelocity) Target->SetPhysicsMaxAngularVelocityInRadians(SourceBI->GetMaxAngularVelocityInRadians());
    TargetBI->SleepFamily = SourceBI->SleepFamily;
    TargetBI->PositionSolverIterationCount = SourceBI->PositionSolverIterationCount;
    TargetBI->VelocitySolverIterationCount = SourceBI->VelocitySolverIterationCount;
    //TargetBI->SetMaxDepenetrationVelocity(SourceBI->MaxDepenetrationVelocity);
    TargetBI->InertiaTensorScale = SourceBI->InertiaTensorScale;
    //if (SourceBI->bOverrideWalkableSlopeOnInstance) Target->SetWalkableSlopeOverride(Source->GetWalkableSlopeOverride());
    TargetBI->CustomSleepThresholdMultiplier = SourceBI->CustomSleepThresholdMultiplier;
    TargetBI->StabilizationThresholdMultiplier = SourceBI->StabilizationThresholdMultiplier;
    TargetBI->bGenerateWakeEvents = SourceBI->bGenerateWakeEvents;
  }
  Target->SetShouldUpdatePhysicsVolume(Source->GetShouldUpdatePhysicsVolume());

  // Collision.
  if (SourceBI && TargetBI)
  {
    Target->SetNotifyRigidBodyCollision(SourceBI->bNotifyRigidBodyCollision);
  }
  //Target->SetPhysMaterialOverride(SourceBI->PhysMaterialOverride);
  Target->SetGenerateOverlapEvents(Source->GetGenerateOverlapEvents());
  Target->CanCharacterStepUpOn = Source->CanCharacterStepUpOn;
  Target->SetCollisionProfileName(Source->GetCollisionProfileName());
  if (Target->GetCollisionProfileName() == "Custom")
  {
    Target->SetCollisionEnabled(Source->GetCollisionEnabled());
    Target->SetCollisionObjectType(Source->GetCollisionObjectType());
    Target->SetCollisionResponseToChannels(Source->GetCollisionResponseToChannels());
  }
  if (SourceBI && TargetBI)
  {
    Target->SetUseCCD(SourceBI->bUseCCD);
    TargetBI->bIgnoreAnalyticCollisions = SourceBI->bIgnoreAnalyticCollisions;
  }
  Target->bAlwaysCreatePhysicsState = Source->bAlwaysCreatePhysicsState;
  Target->bMultiBodyOverlap = Source->bMultiBodyOverlap;
  Target->bTraceComplexOnMove = Source->bTraceComplexOnMove;
  Target->bReturnMaterialOnMove = Source->bReturnMaterialOnMove;
  Target->SetCanEverAffectNavigation(Source->CanEverAffectNavigation());

  // Tags.
  for (const auto& Tag : Source->ComponentTags)
  {
    Target->ComponentTags.Emplace(Tag);
  }

  // Component Replication.
  Target->SetIsReplicated(Source->GetIsReplicated());

  // HLOD.
#if WITH_EDITORONLY_DATA
  for (const auto& Element : Source->ExcludeForSpecificHLODLevels)
  {
    Target->ExcludeForSpecificHLODLevels.Emplace(Element);
  }
  Target->bEnableAutoLODGeneration = Source->bEnableAutoLODGeneration;
#endif
  Target->bUseMaxLODAsImposter = Source->bUseMaxLODAsImposter;
  Target->bBatchImpostersAsInstances = Source->bBatchImpostersAsInstances;

  // Mobile.
  Target->bReceiveMobileCSMShadows = Source->bReceiveMobileCSMShadows;
}

bool UGenMovementComponent::StepMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Position, float Weight, float PlayRate)
{
  if (!Mesh || !Montage || PlayRate <= 0.f || Position < 0.f)
  {
    return false;
  }

  const float MontageLength = Montage->GetPlayLength();
  if (Position >= MontageLength || MontageLength <= 0.f)
  {
    return false;
  }

  const auto AnimInstance = Mesh->GetAnimInstance();
  if (!AnimInstance)
  {
    return false;
  }

  /* @see UAnimMontage::HasValidSlotSetup is not an exported function.
  if (!Montage->HasValidSlotSetup())
  {
    return false;
  } */

  const auto Skeleton = AnimInstance->CurrentSkeleton;
  if (!Skeleton || !Skeleton->IsCompatible(Montage->GetSkeleton()))
  {
    return false;
  }

  // Enforce "a single montage at once per group" rule.
  FName NewMontageGroupName = Montage->GetGroupName();
#if UE_VERSION_OLDER_THAN(5, 0, 0)
  (AnimInstance->*get(UAnimInstance_StopAllMontagesByGroupName()))(NewMontageGroupName, Montage->BlendIn);
#else
  FMontageBlendSettings BlendSettings;
  BlendSettings.BlendProfile = Montage->BlendProfileIn;
  BlendSettings.Blend = Montage->BlendIn;
  BlendSettings.BlendMode = Montage->BlendModeIn;
  (AnimInstance->*get(UAnimInstance_StopAllMontagesByGroupName()))(NewMontageGroupName, BlendSettings);
#endif

  if (Montage->bEnableRootMotionTranslation || Montage->bEnableRootMotionRotation)
  {
    // Enforce "a single root motion montage at once" rule.
    FAnimMontageInstance* ActiveRootMotionMontageInstance = AnimInstance->GetRootMotionMontageInstance();
    if (ActiveRootMotionMontageInstance)
    {
      ActiveRootMotionMontageInstance->Stop(Montage->BlendIn);
    }
  }

  checkGMC(Position >= 0.f && Position < MontageLength)

  auto MontageInstance = new FAnimMontageInstance(AnimInstance);
  check(MontageInstance)

  MontageInstance->Initialize(Montage);
  MontageInstance->SetPlaying(true);
  MontageInstance->SetPlayRate(PlayRate);

  auto& MontageBlend = MontageInstance->*get(FAnimMontageInstance_Blend());
  MontageBlend.*get(FAlphaBlend_BlendedValue()) = FMath::Clamp(Weight, 0.f, 1.f);
  MontageInstance->bEnableAutoBlendOut = Montage->bEnableAutoBlendOut;
  if (Position < MontageLength - Montage->BlendOut.GetBlendTime())
  {
    MontageBlend.SetBlendOption(Montage->BlendIn.GetBlendOption());
    MontageBlend.SetCustomCurve(Montage->BlendIn.GetCustomCurve());
    MontageBlend.SetBlendTime(Montage->BlendIn.GetBlendTime() * MontageInstance->DefaultBlendTimeMultiplier);
    MontageBlend.SetValueRange(0.f, 1.f);
  }
  else
  {
    MontageBlend.SetBlendOption(Montage->BlendOut.GetBlendOption());
    MontageBlend.SetCustomCurve(Montage->BlendOut.GetCustomCurve());
    MontageBlend.SetBlendTime(Montage->BlendOut.GetBlendTime() * MontageInstance->DefaultBlendTimeMultiplier);
    MontageBlend.SetValueRange(1.f, 0.f);
  }

  MontageInstance->SetPosition(Position);

  AnimInstance->MontageInstances.Add(MontageInstance);
  (AnimInstance->*get(UAnimInstance_ActiveMontagesMap())).Emplace(Montage, MontageInstance);

  if (Montage->HasRootMotion())
  {
    // Set this montage instance as the one providing root motion.
    AnimInstance->*get(UAnimInstance_RootMotionMontageInstance()) = MontageInstance;
  }

  return true;
}

float UGenMovementComponent::GetMontagePosition(USkeletalMeshComponent* Mesh, UAnimMontage* Montage) const
{
  if (!Mesh) return -1.f;

  const auto AnimInstance = Mesh->GetAnimInstance();
  if (!AnimInstance) return -1.f;

  if (Montage)
  {
    const auto& MontageInstances = AnimInstance->MontageInstances;
    for (int32 Index = MontageInstances.Num() - 1; Index >= 0; --Index)
    {
      const auto MontageInstance = MontageInstances[Index];
      checkGMC(MontageInstance)
      if(MontageInstance && Montage == MontageInstance->Montage)
      {
        return MontageInstance->GetPosition();
      }
    }
    return -1.f;
  }

  checkGMC(!Montage)

  const auto& MontageInstances = AnimInstance->MontageInstances;
  for (int32 Index = MontageInstances.Num() - 1; Index >= 0; --Index)
  {
    const auto MontageInstance = MontageInstances[Index];
    checkGMC(MontageInstance)
    if (MontageInstance)
    {
      return MontageInstance->GetPosition();
    }
  }

  return -1.f;
}

float UGenMovementComponent::GetMontageWeight(USkeletalMeshComponent* Mesh, UAnimMontage* Montage) const
{
  if (!Mesh) return -1.f;

  const auto AnimInstance = Mesh->GetAnimInstance();
  if (!AnimInstance) return -1.f;

  if (Montage)
  {
    const auto& MontageInstances = AnimInstance->MontageInstances;
    for (int32 Index = MontageInstances.Num() - 1; Index >= 0; --Index)
    {
      const auto MontageInstance = MontageInstances[Index];
      checkGMC(MontageInstance)
      if(MontageInstance && Montage == MontageInstance->Montage)
      {
        return MontageInstance->GetWeight();
      }
    }
    return -1.f;
  }

  checkGMC(!Montage)

  const auto& MontageInstances = AnimInstance->MontageInstances;
  for (int32 Index = MontageInstances.Num() - 1; Index >= 0; --Index)
  {
    const auto MontageInstance = MontageInstances[Index];
    checkGMC(MontageInstance)
    if (MontageInstance)
    {
      return MontageInstance->GetWeight();
    }
  }

  return -1.f;
}

void UGenMovementComponent::ResetMontages(USkeletalMeshComponent* Mesh)
{
  if (!Mesh) return;

  const auto AnimInstance = Mesh->GetAnimInstance();
  if (!AnimInstance) return;

  for (auto Instance : AnimInstance->MontageInstances)
  {
    delete Instance;
  }

  AnimInstance->MontageInstances.Reset();
  (AnimInstance->*get(UAnimInstance_ActiveMontagesMap())).Reset();
  AnimInstance->*get(UAnimInstance_RootMotionMontageInstance()) = nullptr;
}

bool UGenMovementComponent::IsPlayingMontage(USkeletalMeshComponent* Mesh) const
{
  if (!Mesh) return false;

  const auto AnimInstance = Mesh->GetAnimInstance();
  if (!AnimInstance) return false;

  return AnimInstance->MontageInstances.Num() > 0;
}

bool UGenMovementComponent::IsPlayingRootMotion(USkeletalMeshComponent* Mesh) const
{
  if (Mesh)
  {
    return Mesh->IsPlayingRootMotion();
  }
  return false;
}

FAnimMontageInstance* UGenMovementComponent::GetRootMotionMontageInstance(USkeletalMeshComponent* Mesh) const
{
  if (!Mesh)
  {
    return nullptr;
  }

  const auto AnimScriptInstance = Mesh->GetAnimInstance();
  if (!AnimScriptInstance)
  {
    return nullptr;
  }

  return AnimScriptInstance->GetRootMotionMontageInstance();
}

UAnimMontage* UGenMovementComponent::GetRootMotionMontage(USkeletalMeshComponent* Mesh) const
{
  const auto RootMotionMontageInstance = GetRootMotionMontageInstance(Mesh);
  if (!RootMotionMontageInstance)
  {
    return nullptr;
  }
  return RootMotionMontageInstance->Montage;
}

float UGenMovementComponent::GetMontageBlendInTime(UAnimMontage* Montage) const
{
  return Montage ? Montage->BlendIn.GetBlendTime() : 0.f;
}

float UGenMovementComponent::GetMontageBlendOutTime(UAnimMontage* Montage) const
{
  return Montage ? Montage->BlendOut.GetBlendTime() : 0.f;
}

bool UGenMovementComponent::IsBlendingInAtPosition(float Position, UAnimMontage* Montage) const
{
  if (Position < 0.f || !Montage)
  {
    return false;
  }

  if (Position <= Montage->BlendIn.GetBlendTime())
  {
    return true;
  }

  return false;
}

bool UGenMovementComponent::IsBlendingOutAtPosition(float Position, UAnimMontage* Montage) const
{
  if (Position < 0.f || !Montage)
  {
    return false;
  }

  const float MontageLength = Montage->GetPlayLength();
  if (Position <= MontageLength && Position >= MontageLength - Montage->BlendOut.GetBlendTime())
  {
    return true;
  }

  return false;
}

void UGenMovementComponent::NetLog(const FString& String)
{
  GMC_LOG(Log, TEXT("%s"), *String);
}

void UGenMovementComponent::NetLogWarning(const FString& String)
{
  GMC_LOG(Warning, TEXT("%s"), *String);
}

void UGenMovementComponent::NetLogError(const FString& String)
{
  GMC_LOG(Error, TEXT("%s"), *String);
}
