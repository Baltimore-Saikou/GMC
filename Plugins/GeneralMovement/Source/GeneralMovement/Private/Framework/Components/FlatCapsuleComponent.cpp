// Copyright 2022 Dominik Scherer. All Rights Reserved.

#include "FlatCapsuleComponent.h"
#include "GMC_LOG.h"

UFlatCapsuleComponent::UFlatCapsuleComponent()
{
  ShapeColor = FColor(223, 149, 157, 255);
  CapsuleRotation = FLAT_ROTATION_DEFAULT;
  CapsuleRadius = 22.0f;
  CapsuleHalfHeight = 44.0f;
  bUseEditorCompositing = true;
}

void UFlatCapsuleComponent::ApplyDefaultRotation()
{
  SetGenCapsuleRotation(FLAT_ROTATION_DEFAULT);
}

void UFlatCapsuleComponent::ApplyAltRotation()
{
  SetGenCapsuleRotation(FLAT_ROTATION_ALT);
}

FBoxSphereBounds UFlatCapsuleComponent::CalcBounds(const FTransform& LocalToWorld) const
{
  const FVector BoxExtent = FVector(CapsuleHalfHeight, CapsuleRadius, CapsuleRadius);
  return FBoxSphereBounds(FVector::ZeroVector, BoxExtent, CapsuleHalfHeight).TransformBy(LocalToWorld);
}

void UFlatCapsuleComponent::CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const
{
  CylinderRadius = CapsuleHalfHeight;
  CylinderHalfHeight = CapsuleRadius;
}
