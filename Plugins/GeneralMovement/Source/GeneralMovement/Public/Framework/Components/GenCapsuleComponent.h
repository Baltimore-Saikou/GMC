// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "GenCapsuleComponent.generated.h"

/// General capsule collision that can assume arbitrary rotations. Not directly supported by @see UGenMovementComponent, use either the
/// super-class default capsule or the horizontally oriented sub-class capsule.
UCLASS(NotBlueprintable, meta = (DisplayName = "General Capsule Collision"))
class GMC_API UGenCapsuleComponent : public UCapsuleComponent
{
  GENERATED_BODY()

public:

  UGenCapsuleComponent();

  /// Returns the capsule rotation.
  ///
  /// @returns      FRotator    The current capsule rotation.
  UFUNCTION(BlueprintCallable, Category = "Components|Capsule")
  FRotator GetGenCapsuleRotation() const;

  /// Set the capsule rotation.
  /// @attention Arbitrary rotations are not supported by @see UGenMovementComponent.
  ///
  /// @param        NewRotation        The new rotation of the capsule.
  /// @param        bUpdateOverlaps    Whether the touching array for the owner actor should be updated (only if this shape is registered
  ///                                  and collides).
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "Components|Capsule")
  void SetGenCapsuleRotation(FRotator NewRotation, bool bUpdateOverlaps = true);

  /// Set the size and rotation of the capsule.
  /// @attention Arbitrary rotations are not supported by @see UGenMovementComponent.
  ///
  /// @param        NewRotation        The new rotation of the capsule.
  /// @param        NewRadius          The new radius of the center cylinder.
  /// @param        NewHalfHeight      The new half-height (including the hemispheres).
  /// @param        bUpdateOverlaps    Whether the touching array for the owner actor should be updated (only if this shape is registered
  ///                                  and collides).
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "Components|Capsule")
  void SetGenCapsuleSizeAndRotation(FRotator NewRotation, float NewRadius, float NewHalfHeight, bool bUpdateOverlaps = true);

  ///~ Begin USceneComponent Interface
  FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
  void CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const override;
  ///~ End USceneComponent Interface

  ///~ Begin UPrimitiveComponent Interface.
  FPrimitiveSceneProxy* CreateSceneProxy() override;
  ///~ End UPrimitiveComponent Interface.

  ///~ Begin UShapeComponent Interface
  void UpdateBodySetup() override;
  ///~ End UShapeComponent Interface

protected:

  /// The rotation of the capsule component.
  FRotator CapsuleRotation{0};

  /// The explicit template instantiations are not accessible from the engine binaries so we must copy the template functions that we need
  /// (@see UShapeComponent).
#if UE_VERSION_OLDER_THAN(5, 0, 0)
  template<EShapeBodySetupHelper> bool InvalidateOrUpdateCapsuleBodySetup(UBodySetup*&, bool, const FRotator&, float, float);
#else
  template<EShapeBodySetupHelper, typename BodySetupType> bool InvalidateOrUpdateCapsuleBodySetup(BodySetupType&, bool, const FRotator&, float, float);
#endif
  template<typename ShapeElemType> void CreateShapeBodySetupIfNeeded();
  template<typename ShapeElemType> void AddShapeToGeomArray();
  template<typename ShapeElemType> void SetShapeToNewGeom(const FPhysicsShapeHandle&);
};

FORCEINLINE void UGenCapsuleComponent::SetGenCapsuleRotation(FRotator NewRotation, bool bUpdateOverlaps)
{
  SetGenCapsuleSizeAndRotation(NewRotation, GetUnscaledCapsuleRadius(), GetUnscaledCapsuleHalfHeight(), bUpdateOverlaps);
}

FORCEINLINE FRotator UGenCapsuleComponent::GetGenCapsuleRotation() const
{
  return CapsuleRotation;
}
