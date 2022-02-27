// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "GenPawn.h"
#include "GenMovementReplicationComponent.h"
#include "GenMovementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGMCMovement, Log, All);

DECLARE_STATS_GROUP(TEXT("GMCGeneralMovementComponent_Game"), STATGROUP_GMCGenMovementComp, STATCAT_Advanced);

UENUM(BlueprintType)
enum class EGenRadialImpulseFalloff : uint8
{
  Constant UMETA(DisplayName = "Constant", ToolTip = "The impulse is of constant strength, up to the limit of its range."),
  Linear UMETA(DisplayName = "Linear", ToolTip = "The impulse gets linearly weaker the further away we are from the origin."),
  MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EGenCollisionShape : uint8
{
  VerticalCapsule UMETA(DisplayName = "VerticalCapsule"),
  HorizontalCapsule UMETA(DisplayName = "HorizontalCapsule"),
  Box UMETA(DisplayName = "Box"),
  Sphere UMETA(DisplayName = "Sphere"),
  Invalid UMETA(Hidden),
  MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EAdjustDirection : uint8
{
  Up UMETA(DisplayName = "Up"),
  Down UMETA(DisplayName = "Down"),
  MAX UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct GMC_API FFloorParams
{
  GENERATED_BODY()

  // The distance of the pawn's lower bound to the floor based on the shape trace.
  float GetShapeDistanceToFloor() const
  {
    UE_CLOG(
      !HasValidShapeData(),
      LogGMCMovement,
      Warning,
      TEXT("Accessing uninitialized or invalid floor data with call to FFloorParams::GetShapeDistanceToFloor.")
    )
    return ShapeDistanceToFloor;
  }

  // The distance of the pawn's lower bound to the floor based on the line trace.
  float GetLineDistanceToFloor() const
  {
    UE_CLOG(
      !HasValidLineData(),
      LogGMCMovement,
      Warning,
      TEXT("Accessing uninitialized or invalid floor data with call to FFloorParams::GetLineDistanceToFloor.")
    )
    return LineDistanceToFloor;
  }

  // The 2D distance from the center of the pawn to the impact point of the shape trace.
  float GetDistanceToImpact() const
  {
    UE_CLOG(
      !HasValidShapeData(),
      LogGMCMovement,
      Warning,
      TEXT("Accessing uninitialized or invalid floor data with call to FFloorParams::GetDistanceToImpact.")
    )
    return DistanceToImpact;
  }

  // Update the distance of the pawn to the floor from the shape trace.
  void SetShapeDistanceToFloor(float NewDistanceToFloor)
  {
    ShapeDistanceToFloor = FMath::Abs(NewDistanceToFloor);
  }

  // Update the distance of the pawn to the floor from the line trace.
  void SetLineDistanceToFloor(float NewDistanceToFloor)
  {
    LineDistanceToFloor = FMath::Abs(NewDistanceToFloor);
  }

  // The shape trace hit result.
  FHitResult ShapeHit() const
  {
    // Accessing the shape hit when it did not produce a valid blocking hit is not necessarily an error.
    UE_CLOG(
      !HasData(),
      LogGMCMovement,
      Warning,
      TEXT("Accessing uninitialized data with call to FFloorParams::ShapeHit.")
    )
    return ShapeHitResult;
  }

  // The line trace hit result.
  FHitResult LineHit() const
  {
    // Accessing the line hit when it did not produce a valid blocking hit is not necessarily an error.
    UE_CLOG(
      !HasData(),
      LogGMCMovement,
      Warning,
      TEXT("Accessing uninitialized data with call to FFloorParams::LineHit.")
    )
    return LineHitResult;
  }

  // No data.
  FFloorParams() = default;

  // Constructor which automatically calculates the distance to the floor.
  FFloorParams(const FHitResult& ShapeHit, const FHitResult& LineHit, const FVector& CurrentLocation)
    : ShapeHitResult(ShapeHit),
      LineHitResult(LineHit),
      bHasData(true)
  {
    if (ShapeHit.IsValidBlockingHit())
    {
      ShapeDistanceToFloor = FMath::Abs(CurrentLocation.Z - ShapeHit.Location.Z);
      DistanceToImpact = (ShapeHit.ImpactPoint - CurrentLocation).Size2D();
    }
    if (LineHit.IsValidBlockingHit())
    {
      LineDistanceToFloor = LineHit.Distance;
    }
  }

  // Clear the current floor parameters.
  void Clear()
  {
    ShapeHitResult = FHitResult();
    LineHitResult = FHitResult();
    ShapeDistanceToFloor = -1.f;
    LineDistanceToFloor = -1.f;
    DistanceToImpact = -1.f;
    bHasData = false;
  }

  // Returns true if the struct currently holds a valid shape trace hit.
  bool HasValidShapeData() const
  {
    check(
      ShapeHitResult.IsValidBlockingHit()
        ? ShapeDistanceToFloor >= 0.f && DistanceToImpact >= 0.f && bHasData
        : ShapeDistanceToFloor == -1.f && DistanceToImpact == -1.f
    )
    return ShapeHitResult.IsValidBlockingHit();
  }

  // Returns true if the struct currently holds a valid line trace hit.
  bool HasValidLineData() const
  {
    check(LineHitResult.IsValidBlockingHit() ? LineDistanceToFloor >= 0.f && bHasData : LineDistanceToFloor == -1.f)
    return LineHitResult.IsValidBlockingHit();
  }

  // Returns true if the struct was initialized with some form of data (i.e. was not default constructed).
  bool HasData() const
  {
    return bHasData;
  }

protected:

	UPROPERTY(BlueprintReadOnly, Category = "General Movement Component")
	// The shape trace hit result.
	// 形状跟踪命中结果。
	FHitResult ShapeHitResult;

	UPROPERTY(BlueprintReadOnly, Category = "General Movement Component")
	// The line trace hit result.
	// 线性检测命中结果。
	FHitResult LineHitResult;

	UPROPERTY(BlueprintReadOnly, Category = "General Movement Component")
	// The distance of the pawn's lower bound to the floor based on the shape trace (will be -1 if not valid).
	// 基于形状轨迹的 pawn 下界到地板的距离（如果无效，则为 -1）。
	float ShapeDistanceToFloor{-1.f};

	UPROPERTY(BlueprintReadOnly, Category = "General Movement Component")
	// The distance of the pawn's lower bound to the floor based on the line trace (will be -1 if not valid).
	// 基于线性检测的 pawn 下界到地板的距离（如果无效，则为 -1）。
	float LineDistanceToFloor{-1.f};

	UPROPERTY(BlueprintReadOnly, Category = "General Movement Component")
	// The 2D distance from the center of the pawn to the impact point of the shape trace (will be -1 if not valid).
	// 从 pawn 的中心到形状轨迹的撞击点的 2D 距离（如果无效，则为 -1）。
	float DistanceToImpact{-1.f};

	UPROPERTY()
	// True if the struct was initialized with some form of data.
	// 如果结构是用某种形式的数据初始化的，则为真。
	bool bHasData{false};
};

// General accessor for any type of class member. Useful for modifying variables or calling functions of engine classes that are access
// protected.
template<typename Tag, typename Tag::type Member>
struct GenAccessor { friend typename Tag::type get(Tag) { return Member; } };

// Usage example: modifying private member "a" of class "A" (this is completely standard conform).
#if 0
  class A
  {
    int a = 0;
  };

  // Define the tag for A::a.
  struct A_a
  {
    typedef int A::*type;
    friend type get(A_a);
  };

  // Instantiate the accessor for A::a.
  template struct GenAccessor<A_a, &A::a>;

  // Access and modify "a" from anywhere:
  auto pA = new A;
  pA->*get(A_a()) = 42;
#endif

/// Facilitates the implementation of movement components by providing a basic framework with common utility around the replication system.
UCLASS(ClassGroup = "Movement", HideCategories = ("Velocity"), BlueprintType, Blueprintable)
class GMC_API UGenMovementComponent : public UGenMovementReplicationComponent
{
  GENERATED_BODY()

protected:

  void ReplicatedTick(const FMove& Move, int32 Iteration, bool bIsSubSteppedIteration) override final;
  void SimulatedTick(
    float DeltaTime,
    const FState& SmoothState,
    int32 StartStateIndex,
    int32 TargetStateIndex,
    const TArray<int32>& SkippedStateIndices
  ) override final;

  /// Implement your own movement logic here.
  ///
  /// @param        DeltaTime    The delta time for the current move. Not necessarily the same as the local world delta time.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  void GenReplicatedTick(float DeltaTime);
  virtual void GenReplicatedTick_Implementation(float DeltaTime) {}

  /// Implement additional cosmetic logic for remotely controlled pawns here.
  ///
  /// @param        DeltaTime    The current frame delta time.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, Category = "General Movement Component")
  void GenSimulatedTick(float DeltaTime);
  virtual void GenSimulatedTick_Implementation(float DeltaTime) {}

public:

  /// Returns the timestamp of the move currently being executed.
  ///
  /// @returns      float    The timestamp of the current move.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  float GetMoveTimestamp() const;

  /// Returns the delta time for the current move execution (may be sub-stepped and therefore not equal to World->GetDeltaSeconds).
  ///
  /// @returns      float    The delta time of the current move.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  float GetMoveDeltaTime() const;

  /// Returns the input vector of the move currently being executed. This is the raw input vector reflecting the physical user input.
  ///
  /// @returns      FVector    The input vector of the current move.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FVector GetMoveInputVector() const;

  /// Returns the input velocity saved in the current move. This is always the local velocity and is guaranteed to be equal to the current
  /// pawn state at the beginning of a tick.
  ///
  /// @returns      FVector    The InVelocity saved in the current move.
  FVector GetInVelocity() const;

  /// Returns the input location saved in the current move. This is always the local location and is guaranteed to be equal to the current
  /// pawn state at the beginning of a tick.
  ///
  /// @returns      FVector    The InLocation saved in the current move.
  FVector GetInLocation() const;

  /// Returns the input rotation saved in the current move. This is always the local rotation and is guaranteed to be equal to the current
  /// pawn state at the beginning of a tick.
  ///
  /// @returns      FRotator    The InRotation saved in the current move.
  FRotator GetInRotation() const;

  /// Returns the input control rotation saved in the current move. This is always the local control rotation and is guaranteed to be equal
  /// to the current pawn state at the beginning of a tick.
  ///
  /// @returns      FRotator    The InControlRotation saved in the current move.
  FRotator GetInControlRotation() const;

  /// Returns the in input mode saved in the current move. This is always the local input mode and is guaranteed to be equal to the input
  /// mode that was set at the beginning of a tick.
  ///
  /// @returns      EInputMode    The InInputMode saved in the current move.
  EInputMode GetInInputMode() const;

  /// Returns the output velocity saved in the current move. For remotely controlled server pawns this is the output velocity of the client
  /// move, which when not replicated will either be NAN or, if "bEnsureValidMoveData" is true, zeroed. For locally controlled pawns, this
  /// still contains the default constructor value at the time of move execution and is only saved into the move after the tick function has
  /// finished.
  ///
  /// @returns      FVector    The OutVelocity saved in the current move.
  FVector GetOutVelocity() const;

  /// Returns the output location saved in the current move. For remotely controlled server pawns this is the output location of the client
  /// move, which when not replicated will either be NAN or, if "bEnsureValidMoveData" is true, zeroed. For locally controlled pawns, this
  /// still contains the default constructor value at the time of move execution and is only saved into the move after the tick function has
  /// finished.
  ///
  /// @returns      FVector    The OutLocation saved in the current move.
  FVector GetOutLocation() const;

  /// Returns the output rotation saved in the current move. For remotely controlled server pawns this is the output rotation of the client
  /// move, which when not replicated will either be NAN or, if "bEnsureValidMoveData" is true, zeroed. For locally controlled pawns, this
  /// still contains the default constructor value at the time of move execution and is only saved into the move after the tick function has
  /// finished.
  ///
  /// @returns      FRotator    The OutRotation saved in the current move.
  FRotator GetOutRotation() const;

  /// Returns the output control rotation saved in the current move. For remotely controlled server pawns this is the output control
  /// rotation of the client move, which when not replicated will either be NAN or, if "bEnsureValidMoveData" is true, zeroed. For locally
  /// controlled pawns, this still contains the default constructor value at the time of move execution and is only saved into the move
  /// after the tick function has finished.
  ///
  /// @returns      FRotator    The OutControlRotation saved in the current move.
  FRotator GetOutControlRotation() const;

  /// Returns the out input mode saved in the current move. For remotely controlled server pawns this is never set and should not be used.
  /// For locally controlled pawns, this still contains the default constructor value at the time of move execution and is only saved into
  /// the move after the tick function has finished.
  ///
  /// @returns      EInputMode    The OutInputMode saved in the current move.
  EInputMode GetOutInputMode() const;

  /// Returns the number of the current iteration of a move execution. Only relevant in the context of physics sub-stepping. Usually there
  /// is only 1 iteration to be executed but if the delta time of the move exceeds @see UGenMovementReplicationComponent::MaxTimeStep the
  /// move will be broken down into 2 or more iterations.
  /// 返回移动执行的当前迭代次数。仅与物理分步相关。
  /// 通常只需执行一次迭代，但如果移动的增量时间超过@see UGenMovementReplicationComponent:：MaxTimeStep，则移动将分解为两次或更多次迭代。
  ///
  /// @returns      int32    The number of the iteration that currently being executed. This can be between 1 for the first iteration and
  ///                        @see UGenMovementReplicationComponent::MaxIterations (inclusive).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  int32 GetIterationNumber() const;

  /// Tells us whether we are currently executing a sub-stepped iteration of the replicated tick function. An iteration is sub-stepped if
  /// it is not the last one for a given move.
  /// @see MaxTimeStep
  /// @see MaxIterations
  ///
  /// @returns      bool    True if we are currently executing a move within an iteration that is not the final one, false otherwise (i.e.
  ///                       we are within the last and possibly only iteration).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsSubSteppedIteration() const;

  /// Gets the start state for the current interpolation iteration.
  /// @attention Unlike @see UGenMovementReplicationComponent::GetLastInterpolationStartState this only returns a valid state if the pawn
  /// was actually updated during the current tick.
  ///
  /// @returns      const FState&    The current start state if available, otherwise a default state with no valid data (has timestamp -1).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  const FState& GetCurrentInterpolationStartState() const;

  /// Gets the target state for the current interpolation iteration.
  /// @attention Unlike @see UGenMovementReplicationComponent::GetLastInterpolationTargetState this only returns a valid state if the pawn
  /// was actually updated during the current tick.
  ///
  /// @returns      const FState&    The current target state if available, otherwise a default state with no valid data (has timestamp -1).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  const FState& GetCurrentInterpolationTargetState() const;

  /// Gets all skipped states (if any) for the current interpolation iteration from oldest (at index 0) to newest (at the last index). In
  /// other words, index 0 contains the state after the target state of the last interpolation iteration and the last index contains the
  /// state before the start state of the current interpolation iteration.
  ///
  /// @returns      TArray<FState>    Array containing all skipped states. Will have size 0 if no states were skipped (usually the case).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  TArray<FState> GetSkippedInterpolationStates() const;

#pragma region Physics

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Movement Component", meta = (ClampMin = "0.0001", UIMin = "1"))
  /// The mass to use for velocity calculations in the general movement component (in kg).
  float Mass{100.f};

  /// The delta time used for physics calculations by default.
  ///
  /// @returns      float    The currently set physics delta time.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  float GetPhysDeltaTime() const;

  /// Returns the acceleration accumulated since the last physics reset. The acceleration value is transient and does not persist between
  /// ticks.
  ///
  /// @returns      FVector    The current acceleration.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FVector GetTransientAcceleration() const;

  /// Returns the force accumulated since the last physics reset. The force value is transient and does not persist between ticks.
  ///
  /// @returns      FVector    The current force.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FVector GetTransientForce() const;

  /// Returns the starting velocity that was saved with the last physics reset. Does not persist between ticks.
  ///
  /// @returns      FVector    The velocity saved with the last physics reset.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FVector GetStartVelocity() const;

  /// Returns the starting location that was saved with the last physics reset. Does not persist between ticks.
  ///
  /// @returns      FVector    The location saved with the last physics reset.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FVector GetStartLocation() const;

  /// Set the current velocity. Updates acceleration and force from the passed velocity as well.
  ///
  /// @param        NewVelocity    The new velocity to set.
  /// @param        DeltaTime      The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void UpdateVelocity(const FVector& NewVelocity, float DeltaTime = 0.f);

  /// Set the current acceleration. Updates velocity and force from the passed acceleration as well. The acceleration value is transient and
  /// does not persist between ticks.
  ///
  /// @param        NewAcceleration    The new acceleration to set.
  /// @param        DeltaTime          The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void UpdateAcceleration(const FVector& NewAcceleration, float DeltaTime = 0.f);

  /// Set the current force. Updates velocity and acceleration from the passed force as well. The force value is transient and does not
  /// persist between ticks.
  ///
  /// @param        NewForce     The new force to set.
  /// @param        DeltaTime    The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void UpdateForce(const FVector& NewForce, float DeltaTime = 0.f);

  /// Add to the current velocity. Updates acceleration and force from the new total velocity as well.
  ///
  /// @param        AddVelocity    The velocity to add to the current velocity.
  /// @param        DeltaTime      The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void AddVelocity(const FVector& AddVelocity, float DeltaTime = 0.f);

  /// Add to the current acceleration. Updates velocity and force from the new total acceleration as well. The acceleration value is
  /// transient and does not persist between ticks.
  ///
  /// @param        AddAcceleration    The acceleration to add to the current acceleration.
  /// @param        DeltaTime          The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void AddAcceleration(const FVector& AddAcceleration, float DeltaTime = 0.f);

  /// Add to the current force. Updates velocity and acceleration from the new total force as well. The force value is transient and does
  /// not persist between ticks.
  ///
  /// @param        AddForce     The force to add to the current force.
  /// @param        DeltaTime    The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void AddForce(const FVector& AddForce, float DeltaTime = 0.f);

  /// Adds an impulse to the pawn. The impulse should only be applied for one-frame. If you want to add a continous force to the pawn over
  /// several frames use @see AddForce.
  ///
  /// @param        Impulse       The impulse to apply.
  /// @param        bVelChange    When true, the mass of the pawn will not be taken into account.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void AddImpulse(const FVector& Impulse, bool bVelChange);

  /// Adds a radial force to the pawn.
  ///
  /// @param        Origin       The location where the force originates.
  /// @param        Radius       The radius of the force.
  /// @param        Strength     The strength of the force.
  /// @param        Falloff      The falloff function to use (constant or linear).
  /// @param        DeltaTime    The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void AddRadialForce(const FVector& Origin, float Radius, float Strength, EGenRadialImpulseFalloff Falloff, float DeltaTime = 0.f);
  void AddRadialForce(const FVector& Origin, float Radius, float Strength, ERadialImpulseFalloff Falloff) override;

  /// Adds a radial impulse to the pawn. The impulse should only be applied for one-frame. If you want to add a continous radial force to
  /// the pawn over several frames use @see AddRadialForce.
  ///
  /// @param        Origin        The location where the impulse originates.
  /// @param        Radius        The radius of the impulse.
  /// @param        Strength      The strength of the impulse.
  /// @param        Falloff       The falloff function to use (constant or linear).
  /// @param        bVelChange    When true, the mass of the pawn will not be taken into account.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void AddRadialImpulse(const FVector& Origin, float Radius, float Strength, EGenRadialImpulseFalloff Falloff, bool bVelChange);
  void AddRadialImpulse(const FVector& Origin, float Radius, float Strength, ERadialImpulseFalloff Falloff, bool bVelChange) override;

  /// Sets a new physics delta time and optionally resets start location and velocity with the current pawn state and zeroes acceleration
  /// and force.
  ///
  /// @param        NewDeltaTime    The delta time to use for any future physics calculations.
  /// @param        bReset          Whether to update the start location and start velocity, and zero acceleration and force.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void SetPhysDeltaTime(float NewDeltaTime, bool bReset = true);

#pragma endregion

  /// Updates the velocity based on the difference between the current location of the updated component and the currently set start
  /// location.
  ///
  /// @param        DeltaTime    The time over which the change in location occured. If 0 the current physics delta time will be used.
  /// @param        MaxSize      The max magnitude the newly caluclated velocity can have. If 0 the result will not be clamped.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void UpdateVelocityFromMovedDistance(float DeltaTime = 0.f, float MaxSize = 0.f);

  /// Whether the updated component has moved at all since the last physics reset.
  ///
  /// @returns      bool    True if the current updated component location differs from the currently set start location.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasMoved() const;

  /// Immediately stops movement by zeroing all motion values.
  ///
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void HaltMovement();

  /// Returns the current total speed of the pawn.
  ///
  /// @returns      float    The current speed of the pawn.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetSpeed() const;

  /// Returns the current horizontal speed of the pawn.
  ///
  /// @returns      float    The current horizontal speed  of the pawn (XY-velocity-components).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetSpeedXY() const;

  /// Returns the current vertical speed  of the pawn.
  ///
  /// @returns      float    The current vertical speed of the pawn (Z-velocity-component).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetSpeedZ() const;

  /// Adjust the velocity according to the hit normal.
  ///
  /// @param        Hit          The hit result of the collision.
  /// @param        DeltaTime    The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void AdjustVelocityFromHit(const FHitResult& Hit, float DeltaTime);

  /// Adjust the velocity according to the passed normal.
  ///
  /// @param        Normal       The normal to use for adjustment.
  /// @param        DeltaTime    The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void AdjustVelocityFromNormal(FVector Normal, float DeltaTime);

  /// Generates the same pseudorandom positive float on server and client.
  ///
  /// @param        Max      The max value allowed for generated numbers (should be < 100000 to get passable randomness).
  /// @returns      float    A random integer in the range [0, Max) that is safe to use for a networked game.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GenerateNetRandomNumber(int32 Max) const;

  /// Generates the same pseudorandom angle in degrees on server and client.
  ///
  /// @returns      float    A random angle value in the range [0, 360) that is safe to use for a networked game.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GenerateNetRandomAngle() const;

  /// Rotates the updated component smoothly around its local yaw axis towards the passed direction.
  ///
  /// @param        Direction    The direction which the updated component should rotate towards (does not need to be normalized).
  /// @param        Rate         The rate of rotation i.e. how fast the updated component will rotate. If <= 0.f the target rotation will
  ///                            be applied directly.
  /// @param        void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void RotateYawTowardsDirection(const FVector& Direction, float Rate);

  /// Rotates the root collision component smoothly around its local yaw axis towards the passed direction while trying to adjust the pawn
  /// position to avoid collisions.
  ///
  /// @param        Direction    The direction which the updated component should rotate towards (does not need to be normalized).
  /// @param        Rate         The rate of rotation i.e. how fast the updated component will rotate. If <= 0.f the target rotation will
  ///                            be applied directly.
  /// @returns      bool         True if the new rotation was applied, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool RotateYawTowardsDirectionSafe(const FVector& Direction, float Rate);

  /// Compute how deeply the updated component is immersed in a fluid volume. Range is from 0 (not in fluid) to 1 (fully immersed). Always
  /// returns 0 if the current physics volume does not have the water flag set. Fluids volumes should have "bPhysicsOnContact" enabled,
  /// otherwise this will always return 0 if the immersion depth is smaller than 0.5 (meaning the origin of the pawn is not inside the fluid
  /// volume). Assumes that the pawn enters and leaves the fluid volume vertically (i.e. through a horizontal volume face).
  ///
  /// @returns      float    The current immersion depth.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float ComputeImmersionDepth() const;

  /// Checks if the two passed vectors are pointing in opposite directions.
  ///
  /// @param        D1      The first direction.
  /// @param        D2      The second direction.
  /// @returns      bool    True if the two directions are differing by 90 degrees or more, false otherwise.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static bool DirectionsDiffer(const FVector& D1, const FVector& D2);

  /// Checks if the two passed vector's XY components are pointing in opposite directions.
  ///
  /// @param        D1      The first direction.
  /// @param        D2      The second direction.
  /// @returns      bool    True if the two directions are differing by 90 degrees or more in the XY plane, false otherwise.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static bool DirectionsDifferXY(const FVector& D1, const FVector& D2);

  /// Checks if the two passed vector's Z components are pointing in opposite directions.
  ///
  /// @param        D1      The first direction.
  /// @param        D2      The second direction.
  /// @returns      bool    True if D1 and D2 are pointing in the same Z direction (up or down), false if they are pointing in opposite
  ///                       directions.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static bool DirectionsDifferZ(const FVector& D1, const FVector& D2);

  /// Round a floating point number.
  ///
  /// @param        FloatToRound    The floating point number to round.
  /// @param        Level           The quantization level.
  /// @returns      float           The rounded floating point number.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static float RoundFloat(const float& FloatToRound, EDecimalQuantization Level);

  /// Round an vector.
  ///
  /// @param        VectorToRound    The vector to round.
  /// @param        Level            The quantization level.
  /// @returns      FVector          The rounded vector.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FVector RoundVector(const FVector& VectorToRound, EDecimalQuantization Level);

  /// Round a rotator.
  ///
  /// @param        RotatorToRound    The rotator to round.
  /// @param        Level             The quantization level.
  /// @returns      FRotator          The rounded rotator.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FRotator RoundRotator(const FRotator& RotatorToRound, EDecimalQuantization Level);

  /// Check if the pawn's root component is a UShapeComponent of a supported collision shape type with no scaling. This is what the general
  /// movement component assumes for all calculations. Does not operate (i.e. always returns true) in shipping and test builds which is why
  /// you should never use this check to branch on essential logic, it is merely intended to verify the root component setup.
  ///
  /// 检查 pawn 的根组件是否是受支持的碰撞形状类型的 UShapeComponent 且没有缩放。 这是所有计算的一般运动分量所假定的。
  /// 不能在发布和测试版本中运行（即始终返回 true），这就是为什么您不应该使用此检查来分支基本逻辑，它仅用于验证根组件设置。
  ///
  /// @returns      bool    True if the root component is a supported collision shape, false otherwise.
  ///						如果根组件是受支持的碰撞形状，则为 true，否则为 false。
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasValidRootCollision() const;

  /// Query the collision shape of the pawn.
  /// 查询pawn的碰撞形状。
  ///
  /// @returns      EGenCollisionShape    The shape of the pawn's root collision.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual EGenCollisionShape GetRootCollisionShape() const;

  /// Check if the pawn's root collision shape is a vertical capsule.
  ///
  /// @returns      EGenCollisionShape    True if the pawn has a vertical capsule collision, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasVerticalCapsuleCollision() const;

  /// Check if the pawn's root collision shape is a flat capsule.
  ///
  /// @returns      EGenCollisionShape    True if the pawn has a flat capsule collision, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasHorizontalCapsuleCollision() const;

  /// Check if the pawn's root collision shape is a box.
  ///
  /// @returns      EGenCollisionShape    True if the pawn has a box collision, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasBoxCollision() const;

  /// Check if the pawn's root collision shape is a sphere.
  ///
  /// @returns      EGenCollisionShape    True if the pawn has a sphere collision, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasSphereCollision() const;

  /// Returns the extent of the root component's FCollisionShape in the following vector format:
  /// VerticalCapsule   = (Radius, Radius, HalfHeight)
  /// HorizontalCapsule = (Radius, Radius, HalfHeight)
  /// Box               = (HalfExtentX, HalfExtentY, HalfExtentZ)
  /// Sphere            = (Radius, Radius, Radius)
  /// @attention Keep in mind that the actual height (along the Z-axis) of an un-rotated horizontal capsule is determined by the radius, not by the value of the HalfHeight member.
  ///			 请记住，未旋转的水平胶囊的实际高度（沿 Z 轴）由半径决定，而不是由 HalfHeight 成员的值决定。
  /// 
  /// @note Although it would be more logical for flat capsules to have the radius as Z-component, we don't do it this way due to the manner
  /// in which FCollisionShape handles the extent vector and for easier interaction with existing engine functions.
  /// 虽然平面胶囊将半径作为 Z 分量更合乎逻辑，但由于 FCollisionShape 处理范围矢量的方式以及与现有引擎函数的交互更容易，我们不会这样做。
  ///
  /// @returns      FVector    The extent of the root collision.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FVector GetRootCollisionExtent() const;

  /// Returns the half height of the root component. This is the extent in Z direction of the collision's default position (not rotated),
  /// and not necessarily the same as the "HalfHeight" property of a capsule (e.g. for a horizontal capsule this is the radius).
  /// 返回根组件的半高。 这是碰撞默认位置（未旋转）在 Z 方向的范围，不一定与胶囊的“HalfHeight”属性相同（例如，对于水平胶囊，这是半径）。
  ///
  /// @returns      float    The half height of the un-rotated root collision in Z direction.
  ///						 Z 方向上未旋转的根碰撞的半高。
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetRootCollisionHalfHeight() const;

  /// Returns the half height of the vertical portion of the root collision (0 for horizontal capsules and spheres).
  /// 返回根碰撞垂直部分的半高（水平胶囊和球体为 0）。
  ///
  /// @returns      float    The half height of the vertical portion of the collision.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetRootCollisionOuterHalfHeight() const;

  /// Returns the width of the root component. This is the extent in the XY plane from the center in the given direction. For symmetric
  /// shapes the return value is always the radius (meaning the passed direction does not matter) but for horizontal capsules and box shapes
  /// this is the distance to the boundary of the collision in the passed direction.
  ///
  /// @param        Direction    The direction for which to calculate the width (only relevant for non-symmetric shapes).
  /// @returns      float        The width of the root collision in the given direction.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetRootCollisionWidth(const FVector& Direction) const;

  /// Set the extent of the root component's FCollisionShape in the following vector format:
  /// VerticalCapsule   = (Radius, Radius, HalfHeight)
  /// HorizontalCapsule = (Radius, Radius, HalfHeight)
  /// Box               = (HalfExtentX, HalfExtentY, HalfExtentZ)
  /// Sphere            = (Radius, Radius, Radius)
  /// @attention Keep in mind that the actual height (along the Z-axis) of an un-rotated horizontal capsule is determined by the radius, not
  /// by the value of the HalfHeight member.
  ///
  /// @param        NewExtent          The new extent the root collision should have. Component values may be altered if the given vector
  ///                                  does not constitute a valid extent (@see GetValidExtent).
  /// @param        bUpdateOverlaps    If true, updates touching array for owning actor (if the root component is registered and collides).
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetRootCollisionExtent(const FVector& NewExtent, bool bUpdateOverlaps = true);

  /// Version of @see SetRootCollisionExtent that only applies the change if the new extent will not cause any blocking collision.
  ///
  /// @param        NewExtent          The new extent the root collision should have. Component values may be altered if the given vector
  ///                                  does not constitute a valid extent (@see GetValidExtent).
  /// @param        Tolerance          When testing whether the new extent would cause a blocking collision, this argument is applied as a
  ///                                  scaling factor to the passed extent vector to allow for a more lenient (< 1) or stricter (> 1) test.
  /// @param        bUpdateOverlaps    If true, updates touching array for owning actor (if the root component is registered and collides).
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetRootCollisionExtentSafe(const FVector& NewExtent, float Tolerance = 0.99f, bool bUpdateOverlaps = true);

  /// Sets the half height of the root component. This is the extent in Z direction of the collision's default position (not rotated), and
  /// not necessarily the same as the "HalfHeight" property of a capsule (e.g. for a horizontal capsule this is the radius).
  ///
  /// @param        NewHalfHeight      The new half height the root collision should have. May get clamped to ensure a valid extent.
  /// @param        bUpdateOverlaps    If true, updates touching array for owning actor (if the root component is registered and collides).
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetRootCollisionHalfHeight(float NewHalfHeight, bool bUpdateOverlaps = true);

  /// Version of @see SetRootCollisionHalfHeight that only applies the change if the new extent with the passed half height value will not
  /// cause any blocking collision.
  ///
  /// @param        NewHalfHeight      The new half height the root collision should have. May get clamped to ensure a valid extent.
  /// @param        Tolerance          When testing whether the new extent would cause a blocking collision, this argument is applied as a
  ///                                  scaling factor to allow for a more lenient (< 1) or stricter (> 1) test. Keep in mind that the
  ///                                  scaling is applied to the whole extent vector, not just the half height (i.e. Z-extent component).
  /// @param        bUpdateOverlaps    If true, updates touching array for owning actor (if the root component is registered and collides).
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetRootCollisionHalfHeightSafe(float NewHalfHeight, float Tolerance = 0.99f, bool bUpdateOverlaps = true);

  /// Sets the rotation of the root collision.
  ///
  /// @param        NewRotation    The new rotation of the root collision.
  /// @returns      void
  virtual void SetRootCollisionRotation(const FQuat& NewRotation);

  /// Sets the rotation of the root collision.
  ///
  /// @param        NewRotation    The new rotation of the root collision.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetRootCollisionRotation(const FRotator& NewRotation);

  /// Returns the rotation of the root collision.
  ///
  /// @returns      FRotator    The current rotation of the root collision.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FRotator GetRootCollisionRotation() const;

  /// Version of @see SetRootCollisionRotation that only sets the new rotation if it will not cause any blocking collision.
  ///
  /// @param        NewRotation    The new rotation of the root collision.
  /// @param        Tolerance      When testing whether the new rotation would cause a blocking collision, this argument is applied as a
  ///                              scaling factor to the collision extent to allow for a more lenient (< 1) or stricter (> 1) test.
  /// @returns      void
  virtual void SetRootCollisionRotationSafe(const FQuat& NewRotation, float Tolerance = 0.99f);

  /// Version of @see SetRootCollisionRotation that only sets the new rotation if it will not cause any blocking collision.
  ///
  /// @param        NewRotation    The new rotation of the root collision.
  /// @param        Tolerance      When testing whether the new rotation would cause a blocking collision, this argument is applied as a
  ///                              scaling factor to the collision extent to allow for a more lenient (< 1) or stricter (> 1) test.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetRootCollisionRotationSafe(const FRotator& NewRotation, float Tolerance = 0.99f);

  /// Generate an FCollisionShape with the passed extent from an EGenCollisionShape value. The extent format is as follows:
  /// VerticalCapsule   = (Radius, Radius, HalfHeight)
  /// HorizontalCapsule = (Radius, Radius, HalfHeight)
  /// Box               = (HalfExtentX, HalfExtentY, HalfExtentZ)
  /// Sphere            = (Radius, Radius, Radius)
  /// @attention Keep in mind that FCollisionShape does not differentiate between vertical and horizontal capsules.
  ///
  /// @param        CollisionShape     The collision shape we want an FCollisionShape for.
  /// @param        Extent             The extent the FCollisionShape should have. Component values may be altered if the given vector does
  ///                                  not constitute a valid extent (@see GetValidExtent).
  /// @returns      FCollisionShape    The FCollisionShape equivalent to the passed EGenCollisionShape.
  virtual FCollisionShape GetFrom(EGenCollisionShape CollisionShape, const FVector& Extent) const;

  /// Adds the inherent rotation of the general capsule to the passed rotation if applicable. Useful when executing traces for a flat
  /// capsule with an FCollisionShape capsule.
  ///
  /// @param        Rotation    The rotation value to add the flat capsule rotation to.
  /// @returns      FQuat       The passed rotation value rotated by the general capsule rotation. Simply returns the original rotation if
  ///                           the pawn does not have a @see UGenCapsuleComponent as root collision shape.
  virtual FQuat AddGenCapsuleRotation(const FQuat& Rotation) const;

  /// Adds the inherent rotation of the general capsule to the passed rotation if applicable. Useful when executing traces for a flat
  /// capsule with an FCollisionShape capsule.
  ///
  /// @param        Rotation    The rotation value to add the flat capsule rotation to.
  /// @returns      FQuat       The passed rotation value rotated by the general capsule rotation. Simply returns the original rotation if
  ///                           the pawn does not have a @see UGenCapsuleComponent as root collision shape.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FRotator AddGenCapsuleRotation(const FRotator& Rotation) const;

  /// Returns the passed vector as a valid extent for the passed collision shape. A valid extent vector has no zero or nearly zero
  /// components, no negative components and has the same values for components that refer to the same property (e.g. for a sphere all
  /// components have the same value representing the radius). For capsules the half height cannot be smaller than the radius. If there are
  /// different values for the same property in the passed vector the largest value will be chosen.
  ///
  /// @param        CollisionShape    The target collision type (must be a valid shape).
  /// @param        Extent            The extent to validate.
  /// @returns      FVector           A vector that is a valid extent for the passed collision shape (@see GetRootCollisionExtent for the
  ///                                 format).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FVector GetValidExtent(EGenCollisionShape CollisionShape, const FVector& Extent) const;

  /// Checks if the passed vector is a valid extent for the passed collision shape. Refer to @see GetValidExtent for information on what
  /// constitutes a valid extent vector.
  ///
  /// @param        CollisionShape    The target collision type (must be a valid shape).
  /// @param        Extent            The extent to check.
  /// @returns      bool              True if the passed vector is a valid extent for the given collision shape (@see GetRootCollisionExtent
  ///                                 for the format), false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsValidExtent(EGenCollisionShape CollisionShape, const FVector& Extent) const;

  /// Calculates the distance from the center of the root collision to its edge for the given direction in the XY-plane.
  ///
  /// @param        Direction    The direction to check (only the XY-components will be considered).
  /// @returns      float        The distance from the center of the root collision to its boundary in the given direction.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float ComputeDistanceToRootCollisionBoundaryXY(const FVector& Direction) const;

  /// Get the maximum 2D distance from the center to the outer boundary of the root collision.
  ///
  /// @returns      float    The maximum distance to the outer boundary of the collision.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetMaxDistanceToRootCollisionBoundaryXY() const;

  /// Calculates the 2D location of the impact point as percentage between the center (= 0) and the outer boundary (= 1) of the pawn's root
  /// collision.
  ///
  /// @param        ImpactPoint    The location of the impact.
  /// @returns      float          The calculated percentage. Can be > 1 if the impact point lies outside of the collision boundaries.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float ComputeDistanceRootCollisionToImpactPercentXY(const FVector& ImpactPoint) const;

  /// Does a shape trace based on the type of collision that the pawn has starting at the root component's location.
  ///
  /// @param        Direction           The direction in which to trace (does not need to be normalized).
  /// @param        TraceLength         The length of the trace i.e. how far to sweep.
  /// @param        Extent              The extent the trace shape should have (@see GetRootCollisionExtent for the format). Uses the
  ///                                   current extent of the root collision if a zero vector is passed.
  /// @param        Rotation            The rotation of the trace shape.
  /// @param        CollisionChannel    The collision channel to use for sweeping.
  /// @returns      FHitResult          The hit result of the sweep.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "SweepRootCollisionSingleByChannel"))
  virtual FHitResult K2_SweepRootCollisionSingleByChannel_Direction(
    const FVector& Direction,
    float TraceLength,
    const FVector& Extent = FVector::ZeroVector,
    const FRotator& Rotation = FRotator::ZeroRotator,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;
  virtual FHitResult SweepRootCollisionSingleByChannel(
    const FVector& Direction,
    float TraceLength,
    const FVector& Extent = FVector::ZeroVector,
    const FQuat& Rotation = FQuat::Identity,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;

  /// Does a shape trace based on the type of collision that the pawn has.
  ///
  /// @param        TraceStart          The start point of the trace.
  /// @param        TraceEnd            The target point of the trace.
  /// @param        Extent              The extent the trace shape should have (@see GetRootCollisionExtent for the format). Uses the
  ///                                   current extent of the root collision if a zero vector is passed.
  /// @param        Rotation            The rotation of the trace shape.
  /// @param        CollisionChannel    The collision channel to use for sweeping.
  /// @returns      FHitResult          The hit result of the sweep.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "SweepRootCollisionSingleByChannel"))
  virtual FHitResult K2_SweepRootCollisionSingleByChannel(
    const FVector& TraceStart,
    const FVector& TraceEnd,
    const FVector& Extent = FVector::ZeroVector,
    const FRotator& Rotation = FRotator::ZeroRotator,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;
  virtual FHitResult SweepRootCollisionSingleByChannel(
    const FVector& TraceStart,
    const FVector& TraceEnd,
    const FVector& Extent = FVector::ZeroVector,
    const FQuat& Rotation = FQuat::Identity,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;

  /// Does a multi shape trace based on the type of collision that the pawn has starting at the root component's location.
  ///
  /// @param        Direction             The direction in which to trace (does not need to be normalized).
  /// @param        TraceLength           The length of the trace i.e. how far to sweep.
  /// @param        Extent                The extent the trace shape should have (@see GetRootCollisionExtent for the format). Uses the
  ///                                     current extent of the root collision if a zero vector is passed.
  /// @param        Rotation              The rotation of the trace shape.
  /// @param        CollisionChannel      The collision channel to use for sweeping.
  /// @returns      TArray<FHitResult>    The hit results of the sweep.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "SweepRootCollisionMultiByChannel"))
  virtual TArray<FHitResult> K2_SweepRootCollisionMultiByChannel_Direction(
    const FVector& Direction,
    float TraceLength,
    const FVector& Extent = FVector::ZeroVector,
    const FRotator& Rotation = FRotator::ZeroRotator,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;
  virtual TArray<FHitResult> SweepRootCollisionMultiByChannel(
    const FVector& Direction,
    float TraceLength,
    const FVector& Extent = FVector::ZeroVector,
    const FQuat& Rotation = FQuat::Identity,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;

  /// Does a multi shape trace based on the type of collision that the pawn has.
  ///
  /// @param        TraceStart            The start point of the trace.
  /// @param        TraceEnd              The target point of the trace.
  /// @param        Extent                The extent the trace shape should have (@see GetRootCollisionExtent for the format). Uses the
  ///                                     current extent of the root collision if a zero vector is passed.
  /// @param        Rotation              The rotation of the trace shape.
  /// @param        CollisionChannel      The collision channel to use for sweeping.
  /// @returns      TArray<FHitResult>    The hit result of the sweep.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "SweepRootCollisionMultiByChannel"))
  virtual TArray<FHitResult> K2_SweepRootCollisionMultiByChannel(
    const FVector& TraceStart,
    const FVector& TraceEnd,
    const FVector& Extent = FVector::ZeroVector,
    const FRotator& Rotation = FRotator::ZeroRotator,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;
  virtual TArray<FHitResult> SweepRootCollisionMultiByChannel(
    const FVector& TraceStart,
    const FVector& TraceEnd,
    const FVector& Extent = FVector::ZeroVector,
    const FQuat& Rotation = FQuat::Identity,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;

  /// Executes multiple vertically aligned line traces from the bottom of the pawn's root collision up to the passed height.
  ///
  /// @param        Direction             The direction in which to trace (does not need to be normalized).
  /// @param        TraceLength           The length of the line traces.
  /// @param        MaxHeight             Only apply up to this height, starting from the lowest point of the root collision.
  /// @param        Resolution            How many traces to apply within the space between the bottom of the root collision and the passed
  ///                                     height.
  /// @param        CollisionChannel      The collision channel to use for tracing.
  /// @returns      TArray<FHitResult>    The hit results of all traces with index 0 containing the hit result of the lowest line trace and
  ///                                     the last index containing the hit result of the highest line trace.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual TArray<FHitResult> LineTracesAlignedVertical(
    const FVector& Direction,
    float TraceLength,
    float MaxHeight,
    int32 Resolution,
    ECollisionChannel CollisionChannel = ECC_Pawn
  ) const;

  /// Tries to get the pawn unstuck by applying a very small location delta up and down to resolve the penetration.
  ///
  /// @returns      FHitResult    The hit result of the downward sweep.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FHitResult AutoResolvePenetration();

  /// Checks if the passed component is movable.
  ///
  /// @param        Component    The component to check.
  /// @returns      bool         True if the passed component has the EComponentMobility::Movable flag set, false otherwise.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static bool IsMovable(UPrimitiveComponent* Component);

  /// Tries to retrieve the linear velocity of the passed component.
  ///
  /// @param        Component    The component to check.
  /// @returns      FVector      The linear velocity of the component.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FVector GetLinearVelocity(UPrimitiveComponent* Component);

  /// Computes the tangential velocity at the passed location inflicted by the passed component.
  ///
  /// @param        WorldLocation    The location in world space that the tangential velocity should be calculated for.
  /// @param        Component        The component that exerts the force.
  /// @returns      FVector          The tangential velocity at the passed world location.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FVector ComputeTangentialVelocity(const FVector& WorldLocation, UPrimitiveComponent* Component);

  /// Can be used to add the linear and angular velocity of the passed component to the pawn. No velocity is added if bAddLinearVelocity and
  /// bAddTangentialVelocity are both false.
  ///
  /// @param        Source                 The component of which the velocity should be imparted.
  /// @param        bAddLinearVelocity     Whether the linear velocity of the source component should be imparted.
  /// @param        bAddAngularVelocity    Whether the angular velocity of the source component should be imparted.
  /// @param        DeltaTime              The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void ImpartVelocityFrom(UPrimitiveComponent* Source, bool bAddLinearVelocity, bool bAddAngularVelocity, float DeltaTime);

  /// Returns the position of the lowest point of the updated component.
  ///
  /// @returns      FVector    The lower bound of the updated component.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FVector GetLowerBound() const;

  /// Calculates the opposing force to the passed velocity caused by air resistance.
  ///
  /// @param        CurrentVelocity    The current velocity in m/s.
  /// @param        DragCoefficient    The drag coefficient.
  /// @returns      FVector            The force opposing the current velocity in N.
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FVector CalculateAirResistance(const FVector& CurrentVelocity, float DragCoefficient);

  /// Calculates the opposing force to the passed velocity cause by rolling resistance.
  /// 计算由滚动阻力引起的对通过速度的反作用力。
  ///
  /// @param        CurrentVelocity       The current velocity in m/s.
  /// @param        PawnMass              The mass of the pawn in kg.
  /// @param        GravityZ              The gravity Z-component in m/s^2.
  /// @param        RollingCoefficient    The rolling resistance coefficient.
  ///									  滚动阻力系数。
  /// @returns      FVector               The force opposing the current velocity in N.
  ///									  与当前速度相反的力，单位为 N。
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FVector CalculateRollingResistance(const FVector& CurrentVelocity, float PawnMass, float GravityZ, float RollingCoefficient);

  /// Returns the normal of the plane defined by the passed direction and the world up-vector.
  /// 返回由传递的方向和世界向上矢量定义的平面的法线。
  ///
  /// @param        Direction    The direction (does not need to be normalized).
  ///							 方向（不需要归一化）。
  /// @returns      FVector      The cross product of the direction and the world up-vector.
  ///							 方向和世界向上向量的叉积。
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static FVector GetPlaneNormalWithWorldZ(const FVector& Direction);

  /// Does a shape trace of the current root collision downward to update the floor parameters.
  /// 向下跟踪当前根碰撞的形状以更新地板参数。
  ///
  /// @param        Floor          The floor parameters to update.
  ///							   要更新的地板参数。
  /// @param        TraceLength    The length of the trace.
  ///							   射线的长度。
  /// @returns      bool           If false, the pawn's position was adjusted (when the initial sweep started in penetration).
  ///							   如果为 false，则调整 pawn 的位置（当初始扫描开始于穿透时）。
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool UpdateFloor(UPARAM(ref) FFloorParams& Floor, float TraceLength);

  /// Checks if a given point is closer to the collision center than the tolerance allows. Usually used to discard hits that are very close
  /// to the edge of the vertical portion of the collision shape.
  ///
  /// @param        Location       The location of the root collision.
  /// @param        Point          The point to test.
  /// @param        Tolerance      The edge tolerance.
  /// @returns      bool           True if the point is further towards the center of the collision than the tolerance allows.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsWithinEdgeTolerance(const FVector& Location, const FVector& Point, float Tolerance) const;

  /// Linearly interpolates the current half height of the root collision towards the target value. The half height is the Z-extent of the
  /// unrotated collision shape so for flat capsules and spheres the height is the radius property of the class.
  /// @attention The new half height is set through @see SetRootCollisionExtentSafe meaning no changes will be applied if the new extent
  /// would cause a blocking collision.
  ///
  /// @param        TargetHalfHeight    The target value for the interpolation. Must be greater than 0.
  /// @param        InterpSpeed         How quickly the target value should be reached.
  /// @param        InterpTolerance     Factor the test extent is scaled with (@see SetRootCollisionExtentSafe).
  /// @param        DeltaTime           The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @param        bAdjustPosition     If true, the updated component will be moved up/down (depending on the value of "AdjustDirection")
  ///                                   by the amount the half height has changed.
  /// @param        AdjustDirection     Whether the updated component should move up or down. Only relevant if "bAdjustPosition" is true.
  /// @returns      float               The absolute difference in half height that was actually applied to the root collision shape.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float LerpRootCollisionHalfHeight(
    float TargetHalfHeight,
    float InterpSpeed,
    float InterpTolerance = 0.99f,
    float DeltaTime = 0.f,
    bool bAdjustPosition = false,
    EAdjustDirection AdjustDirection = EAdjustDirection::Up
  );

  /// Linearly interpolates the current width of the root collision towards the target value. For vertical capsules and spheres the width is
  /// the radius, for flat capsules it is the half height property of the class. For box collisions the value operated on is the X-extent
  /// if "bInterpBoxY" is false, otherwise it is the Y-extent.
  /// @attention The new width is set through @see SetRootCollisionExtentSafe meaning no changes will be applied if the new extent would
  /// cause a blocking collision.
  ///
  /// @param        TargetWidth        The target value for the interpolation. Must be greater than 0.
  /// @param        InterpSpeed        How quickly the target value should be reached.
  /// @param        InterpTolerance    Factor the test extent is scaled with (@see SetRootCollisionExtentSafe).
  /// @param        DeltaTime          The delta time to use. If <= 0 the currently set physics delta time will be used.
  /// @param        bInterpBoxY        Only relevant if the root collision is a box shape. If true the Y-extent of the box collision will be
  ///                                  interpolated instead of the X-extent.
  /// @returns      float              The absolute difference in width that was actually applied to the root collision shape.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float LerpRootCollisionWidth(
    float TargetWidth,
    float InterpSpeed,
    float InterpTolerance = 0.99f,
    float DeltaTime = 0.f,
    bool bInterpBoxY = false
  );

  /// Replaces the current root collision of the pawn with a new one of the passed shape and extent. The current component settings (for
  /// collision, physics, rendering etc.) will be adopted and the original root component will be destroyed. Only updates the extent and the
  /// name if the root component already has the passed collision shape.
  /// @attention Some settings cannot be copied to the new root component and may have to be reapplied manually afterwards (walkable slope
  /// override, phys material override, max depenetration velocity, custom primitive data, asset user data).
  ///
  /// @param        NewCollisionShape    The new collision type to use (must be a valid shape).
  /// @param        Extent               The extent of the new root component (@see GetRootCollisionExtent for the format). Component values
  ///                                    may be altered if the given vector does not constitute a valid extent (@see GetValidExtent).
  /// @param        Name                 The name of the new root component. The new name must be unique and different from the name of the
  ///                                    root component when the actor was first spawned.
  /// @returns      USceneComponent*     The new root component. May be equal to the old root component if the pawn already had the passed
  ///                                    collision shape.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual USceneComponent* SetRootCollisionShape(EGenCollisionShape NewCollisionShape, const FVector& Extent, FName Name);

  /// Version of @see SetRootCollisionShape that only applies the change if the new shape will not cause any blocking collision.
  ///
  /// @param        NewCollisionShape    The new collision type to use (must be a valid shape).
  /// @param        Extent               The extent of the new root component (@see GetRootCollisionExtent for the format). Component values
  ///                                    may be altered if the given vector does not constitute a valid extent (@see GetValidExtent).
  /// @param        Name                 The name of the new root component. The new name must be unique and different from the name of the
  ///                                    root component when the actor was first spawned.
  /// @param        Tolerance            When testing whether the new shape would cause a blocking collision, this argument is applied as a
  ///                                    scaling factor to the extent to allow for a more lenient (< 1) or stricter (> 1) test.
  /// @returns      USceneComponent*     The new root component. May be equal to the old root component if the pawn already had the passed
  ///                                    collision shape.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual USceneComponent* SetRootCollisionShapeSafe(
    EGenCollisionShape NewCollisionShape,
    const FVector& Extent,
    FName Name,
    float Tolerance = 0.99f
  );

  /// Tests whether the passed location would be a valid (collision-free) spot for the passed shape.
  ///
  /// @param        CollisionShape      The collision shape to test.
  /// @param        Extent              The extent of the collision to test. Must be a valid extent vector as defined by @see IsValidExtent.
  /// @param        Location            The test location.
  /// @param        Rotation            The test rotation.
  /// @param        OutHit              The hit result of the test.
  /// @param        CollisionChannel    The test collision channel.
  /// @param        Tolerance           Scale the passed extent by this factor to allow for a more lenient (< 1) or stricter (> 1) test.
  /// @returns      bool                True if the location is a valid spot for the test shape with the given rotation and extent. Always
  ///                                   returns false if an invalid extent vector was passed. Always returns true if the passed tolerance
  ///                                   was <= 0 and the passed extent was valid.
  virtual bool IsValidPosition(
    EGenCollisionShape CollisionShape,
    const FVector& Extent,
    const FVector& Location,
    const FQuat& Rotation,
    FHitResult& OutHit,
    ECollisionChannel CollisionChannel = ECC_Pawn,
    float Tolerance = 1.f
  ) const;

  /// Tests whether the passed location would be a valid (collision-free) spot for the passed shape.
  ///
  /// @param        CollisionShape      The collision shape to test.
  /// @param        Extent              The extent of the collision to test. Must be a valid extent vector as defined by @see IsValidExtent.
  /// @param        Location            The test location.
  /// @param        Rotation            The test rotation.
  /// @param        OutHit              The hit result of the test.
  /// @param        CollisionChannel    The test collision channel.
  /// @param        Tolerance           Scale the passed extent by this factor to allow for a more lenient (< 1) or stricter (> 1) test.
  /// @returns      bool                True if the location is a valid spot for the test shape with the given rotation and extent. Always
  ///                                   returns false if an invalid extent vector was passed. Always returns true if the passed tolerance
  ///                                   was <= 0 and the passed extent was valid.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsValidPosition(
    EGenCollisionShape CollisionShape,
    const FVector& Extent,
    const FVector& Location,
    const FRotator& Rotation,
    FHitResult& OutHit,
    ECollisionChannel CollisionChannel = ECC_Pawn,
    float Tolerance = 1.f
  ) const;

  /// Determines whether the updated component can be moved through movement physics.
  ///
  /// @returns      bool    True if the updated component can move, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool CanMove() const;

  /// Check if the pawn can receive directional movement input.
  ///
  /// @returns      bool    True if this pawn receives move input, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool HasMoveInputEnabled() const;

  /// Check if the hit object is a valid base for a pawn to stand on.
  ///
  /// @param        Hit     The hit result to check.
  /// @returns      bool    True if the hit object is a valid base for a pawn, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool CanStepUp(const FHitResult& Hit) const;

  /// Used by @see SetRootCollisionShape. Copies the settings of the source component to the target component. Can be overriden for custom
  /// behaviour or if a required setting is not copied by default.
  ///
  /// @param        Source    The component to copy the settings from.
  /// @param        Target    The component to copy the settings to.
  /// @returns      void
  virtual void CopyComponentSettings(UShapeComponent* Source, UShapeComponent* Target);

  /// Plays a montage at the passed position. Suitable for use within the replicated tick function. When in a networked context the inputs
  /// for "Position" and "Weight" can be bound to member variables and updated after the mesh pose was ticked. These values can then be used
  /// in the next tick to drive the montage further. The function allocates a new montage with each call so all montage data should be reset
  /// before montages are stepped again in the next tick (@see ResetMontages). This will also enforce frame independency for any other
  /// montage related data in the anim instance (important for client replay).
  /// 在传球位置播放蒙太奇。 适合在复制刻度函数中使用。 在网络环境中，“Position”和“Weight”的输入可以绑定到成员变量并在网格姿势被勾选后更新。
  /// 然后可以在下一个刻度中使用这些值来进一步推动蒙太奇。
  /// 该函数为每次调用分配一个新的蒙太奇，因此所有蒙太奇数据都应在下一个滴答中再次步进蒙太奇之前重置（@see ResetMontages）。
  /// 这还将强制动画实例中任何其他蒙太奇相关数据的帧独立性（对于客户端回放很重要）。
  /// 
  /// @attention For Blueprint users: Unlike the built-in "PlayMontage" node this is not a latent action so the function needs to be called
  /// each tick with the advanced position and weight if the montage should play continuously.
  /// @attention 蓝图用户：与内置的“PlayMontage”节点不同，这不是潜在动作，因此如果蒙太奇应该连续播放，则需要在每个刻度上调用高级位置和权重的函数。
  /// 
  /// @attention This function sets up a montage for play with the appropriate values but montages are only actually stepped when the mesh
  /// pose is ticked.
  /// @attention 此函数设置蒙太奇以使用适当的值进行播放，但蒙太奇仅在网格姿势被勾选时才会实际步进。
  ///
  /// @param        Mesh        The skeletal mesh of the owning pawn.
  /// @param        Montage     The montage to play.
  /// @param        Position    The position to play at in the montage. Must be >= 0 and < montage length for the montage to be stepped.
  /// @param        Weight      The blended value of the montage's alpha blend. Will be clamped to a min value of 0 and a max value of 1.
  ///                           蒙太奇的 alpha 混合的混合值。 将被限制为最小值 0 和最大值 1。
  /// @param        PlayRate    The rate at which the montage should advance. Must be greater than 0.
  /// @returns      bool        True if the montage will be stepped (i.e. is playing), false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool StepMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Position, float Weight, float PlayRate = 1.f);

  /// Gets the position the passed montage is currently at (if playing). If nullptr is passed the position of the first found currently
  /// playing montage is returned. Includes montages that are blending out.
  ///
  /// @param        Mesh       The skeletal mesh of the owning pawn.
  /// @param        Montage    The montage to query the position for. If nullptr the first found currently playing montage will be queried.
  /// @returns      float      The position of the queried montage. Will return -1 if the passed montage is not playing (or if no montage is
  ///                          playing at all).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetMontagePosition(USkeletalMeshComponent* Mesh, UAnimMontage* Montage = nullptr) const;

  /// Gets the current weight/blended value of the passed montage (if playing). If nullptr is passed the weight of the first found currently
  /// playing montage is returned. Includes montages that are blending out.
  ///
  /// @param        Mesh       The skeletal mesh of the owning pawn.
  /// @param        Montage    The montage to query the weight for. If nullptr the first found currently playing montage will be queried.
  /// @returns      float      The blended value of the queried montage. Will return -1 if the passed montage is not playing (or if no
  ///                          montage is playing at all).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetMontageWeight(USkeletalMeshComponent* Mesh, UAnimMontage* Montage = nullptr) const;

  /// Clears @see UAnimInstance::MontageInstances and @see UAnimInstance::ActiveMontagesMap.
  ///
  /// @param        Mesh    The skeletal mesh of the owning pawn.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void ResetMontages(USkeletalMeshComponent* Mesh);

  /// Checks whether we currently have any montages playing. Includes montages that are blending out.
  ///
  /// @param        Mesh    The skeletal mesh of the owning pawn.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsPlayingMontage(USkeletalMeshComponent* Mesh) const;

  /// Whether the pawn is currently playing root motion.
  ///
  /// @param        Mesh    The skeletal mesh of the owning pawn.
  /// @returns      bool    True if the pawn is playing root motion, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsPlayingRootMotion(USkeletalMeshComponent* Mesh) const;

  /// Returns the currently playing root motion montage instance (if any).
  ///
  /// @param        Mesh                     The skeletal mesh of the owning pawn.
  /// @returns      FAnimMontageInstance*    The current root motion montage instance or nullptr if none is currently playing.
  virtual struct FAnimMontageInstance* GetRootMotionMontageInstance(USkeletalMeshComponent* Mesh) const;

  /// Returns the montage used by the currently playing root motion montage instance (if any).
  ///
  /// @param        Mesh             The skeletal mesh of the owning pawn.
  /// @returns      UAnimMontage*    The current root motion montage or nullptr if none is currently playing.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual UAnimMontage* GetRootMotionMontage(USkeletalMeshComponent* Mesh) const;

  /// Returns the blend in time of the passed montage.
  ///
  /// @param        Montage    The montage to get the blend in time for.
  /// @returns      float      The blend in time of the montage.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetMontageBlendInTime(UAnimMontage* Montage) const;

  /// Returns the blend out time of the passed montage.
  ///
  /// @param        Montage    The montage to get the blend out time for.
  /// @returns      float      The blend out time of the montage.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual float GetMontageBlendOutTime(UAnimMontage* Montage) const;

  /// Whether the passed position is within the blend in range of the montage.
  ///
  /// @param        Position    The position to check. Must be >= 0.
  /// @param        Montage     The montage to check.
  /// @returns      bool        True if the passed position is within the blend in range of the montage, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsBlendingInAtPosition(float Position, UAnimMontage* Montage) const;

  /// Whether the passed position is within the blend out range of the montage.
  /// @attention This will return false if the passed position is greater than the length of the montage.
  ///
  /// @param        Position    The position to check. Must be >= 0.
  /// @param        Montage     The montage to check.
  /// @returns      bool        True if the passed position is within the blend out range of the montage, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual bool IsBlendingOutAtPosition(float Position, UAnimMontage* Montage) const;

  /// Prints the given string to the log with additional debug information pertaining to the network context. Should only be called from GMC
  /// components when there is a valid reference to the owning pawn defined. Does not operate in builds that have logging disabled.
  ///
  /// @param        String    The message to log.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void NetLog(const FString& String = FString(TEXT("Hello")));

  /// Prints the given string to the log as a warning with additional debug information pertaining to the network context. Should only be
  /// called from GMC components when there is a valid reference to the owning pawn defined. Does not operate in builds that have logging
  /// disabled.
  ///
  /// @param        String    The message to log.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void NetLogWarning(const FString& String = FString(TEXT("Hello")));

  /// Prints the given string to the log as an error with additional debug information pertaining to the network context. Should only be
  /// called from GMC components when there is a valid reference to the owning pawn defined. Does not operate in builds that have logging
  /// disabled.
  ///
  /// @param        String    The message to log.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void NetLogError(const FString& String = FString(TEXT("Hello")));

private:

  /// The timestamp of the current move.
  float Timestamp{0.f};
  /// The (sub-stepped) delta time for the current move execution.
  float MoveDeltaTime{0.f};
  /// The current input vector.
  FVector InputVector{0};
  /// The input values saved in the current move.
  FVector InVelocity{0};
  FVector InLocation{0};
  FRotator InRotation{0};
  FRotator InControlRotation{0};
  EInputMode InInputMode{0};
  /// The output values saved in the current move.
  FVector OutVelocity{0};
  FVector OutLocation{0};
  FRotator OutRotation{0};
  FRotator OutControlRotation{0};
  EInputMode OutInputMode{0};
  /// States from the simulated tick. The default state is required so @see GetCurrentInterpolationStartState and
  /// @see GetCurrentInterpolationTargetState can be made Blueprint callable functions.
  const FState* StartStatePtr{nullptr};
  const FState* TargetStatePtr{nullptr};
  TArray<const FState*> SkippedStatePtrs;
  FState DefaultState;
  /// The delta time used for physics calculations.
  float PhysDeltaTime{0.f};
  /// The current acceleration of the pawn in cm/s^2.
  FVector Acceleration{0};
  /// The current force acting on the pawn in kg*cm/s^2.
  FVector Force{0};
  /// The velocity at the beginning of the current update in cm/s.
  FVector Velocity0{0};
  /// The location of the updated component at the beginning of the current update.
  FVector Location0{0};
  /// Info about the floor that is currently located underneath the pawn.
  /// 有关当前位于棋子下方的地板的信息。
  FFloorParams CurrentFloor;
  /// The number of the current move iteration.
  int32 MoveIteration{0};
  /// Whether we are currently within a sub-stepped iteration of a move execution.
  bool bIsSubSteppedMoveIteration{false};
};

FORCEINLINE float UGenMovementComponent::GetMoveTimestamp() const
{
  return Timestamp;
}

FORCEINLINE float UGenMovementComponent::GetMoveDeltaTime() const
{
  return MoveDeltaTime;
}

FORCEINLINE FVector UGenMovementComponent::GetMoveInputVector() const
{
  return InputVector;
}

FORCEINLINE FVector UGenMovementComponent::GetTransientAcceleration() const
{
  return Acceleration;
}

FORCEINLINE FVector UGenMovementComponent::GetTransientForce() const
{
  return Force;
}

FORCEINLINE FVector UGenMovementComponent::GetStartVelocity() const
{
  return Velocity0;
}

FORCEINLINE FVector UGenMovementComponent::GetStartLocation() const
{
  return Location0;
}

FORCEINLINE float UGenMovementComponent::GetSpeed() const
{
  return GetVelocity().Size();
}

FORCEINLINE float UGenMovementComponent::GetSpeedXY() const
{
  return GetVelocity().Size2D();
}

FORCEINLINE float UGenMovementComponent::GetSpeedZ() const
{
  return GetVelocity().Z;
}

FORCEINLINE bool UGenMovementComponent::DirectionsDiffer(const FVector& D1, const FVector& D2)
{
  return (D1 | D2) <= 0.f;
}

FORCEINLINE bool UGenMovementComponent::DirectionsDifferXY(const FVector& D1, const FVector& D2)
{
  return (FVector(D1.X, D1.Y, 0.f) | FVector(D2.X, D2.Y, 0.f)) <= 0.f;
}

FORCEINLINE bool UGenMovementComponent::DirectionsDifferZ(const FVector& D1, const FVector& D2)
{
  return D1.Z * D2.Z <= 0.f;
}

FORCEINLINE FVector UGenMovementComponent::GetInVelocity() const
{
  return InVelocity;
}

FORCEINLINE FVector UGenMovementComponent::GetInLocation() const
{
  return InLocation;
}

FORCEINLINE FRotator UGenMovementComponent::GetInRotation() const
{
  return InRotation;
}

FORCEINLINE FRotator UGenMovementComponent::GetInControlRotation() const
{
  return InControlRotation;
}

FORCEINLINE EInputMode UGenMovementComponent::GetInInputMode() const
{
  return InInputMode;
}

FORCEINLINE FVector UGenMovementComponent::GetOutVelocity() const
{
  return OutVelocity;
}

FORCEINLINE FVector UGenMovementComponent::GetOutLocation() const
{
  return OutLocation;
}

FORCEINLINE FRotator UGenMovementComponent::GetOutRotation() const
{
  return OutRotation;
}

FORCEINLINE FRotator UGenMovementComponent::GetOutControlRotation() const
{
  return OutControlRotation;
}

FORCEINLINE EInputMode UGenMovementComponent::GetOutInputMode() const
{
  return OutInputMode;
}

FORCEINLINE int32 UGenMovementComponent::GetIterationNumber() const
{
  return MoveIteration;
}

FORCEINLINE bool UGenMovementComponent::IsSubSteppedIteration() const
{
  return bIsSubSteppedMoveIteration;
}

#pragma region Accessors

struct UAnimInstance_RootMotionMontageInstance
{
  typedef FAnimMontageInstance* UAnimInstance::*type;
  friend type get(UAnimInstance_RootMotionMontageInstance);
};

struct UAnimInstance_ActiveMontagesMap
{
  typedef TMap<UAnimMontage*, FAnimMontageInstance*> UAnimInstance::*type;
  friend type get(UAnimInstance_ActiveMontagesMap);
};

#if UE_VERSION_OLDER_THAN(5, 0, 0)

struct UAnimInstance_StopAllMontagesByGroupName
{
  typedef void (UAnimInstance::*type)(FName, const FAlphaBlend&);
  friend type get(UAnimInstance_StopAllMontagesByGroupName);
};

#else

struct UAnimInstance_StopAllMontagesByGroupName
{
  typedef void (UAnimInstance::*type)(FName, const FMontageBlendSettings&);
  friend type get(UAnimInstance_StopAllMontagesByGroupName);
};

#endif

struct FAnimMontageInstance_Blend
{
  typedef FAlphaBlend FAnimMontageInstance::*type;
  friend type get(FAnimMontageInstance_Blend);
};

struct FAlphaBlend_BlendedValue
{
  typedef float FAlphaBlend::*type;
  friend type get(FAlphaBlend_BlendedValue);
};

// Accessor for @see UAnimInstance::RootMotionMontageInstance.
template struct GenAccessor<UAnimInstance_RootMotionMontageInstance, &UAnimInstance::RootMotionMontageInstance>;

// Accessor for @see UAnimInstance::ActiveMontagesMap.
template struct GenAccessor<UAnimInstance_ActiveMontagesMap, &UAnimInstance::ActiveMontagesMap>;

// Accessor for @see UAnimInstance::StopAllMontagesByGroupName.
template struct GenAccessor<UAnimInstance_StopAllMontagesByGroupName, &UAnimInstance::StopAllMontagesByGroupName>;

// Accessor for @see FAnimMontageInstance::Blend.
template struct GenAccessor<FAnimMontageInstance_Blend, &FAnimMontageInstance::Blend>;

// Accessor for @see FAlphaBlend::BlendedValue.
template struct GenAccessor<FAlphaBlend_BlendedValue, &FAlphaBlend::BlendedValue>;

#pragma endregion
