// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "GenCapsuleComponent.h"
#include "FlatCapsuleComponent.generated.h"

/// A horizontally oriented capsule component.
UCLASS(ClassGroup = "Collision", Blueprintable, meta = (DisplayName = "Flat Capsule Collision", BlueprintSpawnableComponent))
class GMC_API UFlatCapsuleComponent : public UGenCapsuleComponent
{
  GENERATED_BODY()

public:

  UFlatCapsuleComponent();

  /// The orientations this capsule can have. Note that other values may not work properly with @see UGenMovementComponent.
  inline static const FRotator FLAT_ROTATION_DEFAULT = {90.f, 0.f, 0.f};
  inline static const FRotator FLAT_ROTATION_ALT = {90.f, 90.f, 0.f};

  /// Use the default orientation for this capsule.
  ///
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "Components|Capsule")
  void ApplyDefaultRotation();

  /// Use the alternative orientation for this capsule.
  ///
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "Components|Capsule")
  void ApplyAltRotation();

  FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
  void CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const override;
};
