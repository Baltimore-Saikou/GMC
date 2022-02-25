// Copyright 2022 Dominik Scherer. All Rights Reserved.

#include "GenCapsuleComponent.h"
#include "GMC_LOG.h"

UGenCapsuleComponent::UGenCapsuleComponent()
{
  ShapeColor = FColor(223, 149, 157, 255);
  CapsuleRotation = FRotator::ZeroRotator;
  CapsuleRadius = 22.f;
  CapsuleHalfHeight = 44.f;
  bUseEditorCompositing = true;
}

void UGenCapsuleComponent::SetGenCapsuleSizeAndRotation(
  FRotator NewRotation,
  float NewRadius,
  float NewHalfHeight,
  bool bUpdateOverlaps
)
{
  CapsuleRotation = NewRotation;
  SetCapsuleSize(NewRadius, NewHalfHeight, bUpdateOverlaps);
}

FBoxSphereBounds UGenCapsuleComponent::CalcBounds(const FTransform& LocalToWorld) const
{
  checkfGMC(false, TEXT("Not implemented."))
  return USceneComponent::CalcBounds(LocalToWorld);
}

void UGenCapsuleComponent::CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const
{
  checkfGMC(false, TEXT("Not implemented."))
  return USceneComponent::CalcBoundingCylinder(CylinderRadius, CylinderHalfHeight);
}

void UGenCapsuleComponent::UpdateBodySetup()
{
  if (PrepareSharedBodySetup<UGenCapsuleComponent>())
  {
    bUseArchetypeBodySetup = InvalidateOrUpdateCapsuleBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(
      ShapeBodySetup,
      bUseArchetypeBodySetup,
      CapsuleRotation,
      CapsuleRadius,
      CapsuleHalfHeight
    );
  }
  CreateShapeBodySetupIfNeeded<FKSphylElem>();
  if (!bUseArchetypeBodySetup)
  {
    InvalidateOrUpdateCapsuleBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(
      ShapeBodySetup,
      bUseArchetypeBodySetup,
      CapsuleRotation,
      CapsuleRadius,
      CapsuleHalfHeight
    );
  }
}

FPrimitiveSceneProxy* UGenCapsuleComponent::CreateSceneProxy()
{
  class FDrawCylinderSceneProxy final : public FPrimitiveSceneProxy
  {
  public:
    SIZE_T GetTypeHash() const override
    {
      static size_t UniquePointer;
      return reinterpret_cast<size_t>(&UniquePointer);
    }

    FDrawCylinderSceneProxy(const UGenCapsuleComponent* InComponent)
      : FPrimitiveSceneProxy(InComponent),
        bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected),
        CapsuleRotation(InComponent->CapsuleRotation),
        CapsuleRadius(InComponent->GetUnscaledCapsuleRadius()),
        CapsuleHalfHeight(InComponent->GetUnscaledCapsuleHalfHeight()),
        ShapeColor(InComponent->ShapeColor)
    {
      bWillEverBeLit = false;
    }

    void GetDynamicMeshElements(
      const TArray<const FSceneView*>& Views,
      const FSceneViewFamily& ViewFamily,
      uint32 VisibilityMap,
      FMeshElementCollector& Collector
    ) const override
    {
      const FMatrix& LocalToWorld = GetLocalToWorld();
      const int32 CapsuleSides = FMath::Clamp<int32>(CapsuleRadius / 4.f, 16, 64);
      for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
      {
        if (VisibilityMap & (1 << ViewIndex))
        {
          const FSceneView* View = Views[ViewIndex];
          const FLinearColor DrawCapsuleColor = GetViewSelectionColor(
            ShapeColor,
            *View,
            IsSelected(),
            IsHovered(),
            false,
            IsIndividuallySelected()
          );
          FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
          DrawWireCapsule(
            PDI,
            LocalToWorld.GetOrigin(),
            LocalToWorld.TransformVector(FRotationMatrix(CapsuleRotation).GetScaledAxis(EAxis::X)),
            LocalToWorld.TransformVector(FRotationMatrix(CapsuleRotation).GetScaledAxis(EAxis::Y)),
            LocalToWorld.TransformVector(FRotationMatrix(CapsuleRotation).GetScaledAxis(EAxis::Z)),
            DrawCapsuleColor,
            CapsuleRadius,
            CapsuleHalfHeight,
            CapsuleSides,
            SDPG_World
          );
        }
      }
    }

    FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
    {
      const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();
      const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();
      FPrimitiveViewRelevance Result;
      Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
      Result.bDynamicRelevance = true;
      Result.bShadowRelevance = IsShadowCast(View);
      Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
      return Result;
    }

    uint32 GetMemoryFootprint(void) const override
    {
      return (sizeof(*this) + GetAllocatedSize());
    }

    uint32 GetAllocatedSize(void) const
    {
      return (FPrimitiveSceneProxy::GetAllocatedSize());
    }

  private:
    const uint32 bDrawOnlyIfSelected:1;
    const FRotator CapsuleRotation;
    const float CapsuleRadius;
    const float CapsuleHalfHeight;
    const FColor ShapeColor;
  };

  return new FDrawCylinderSceneProxy(this);
}

#if UE_VERSION_OLDER_THAN(5, 0, 0)
template<EShapeBodySetupHelper UpdateBodySetupAction>
bool UGenCapsuleComponent::InvalidateOrUpdateCapsuleBodySetup(
  UBodySetup*& ShapeBodySetup,
#else
template<EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool UGenCapsuleComponent::InvalidateOrUpdateCapsuleBodySetup(
  BodySetupType& ShapeBodySetup,
#endif
  bool bUseArchetypeBodySetup,
  const FRotator& CapsuleRotation,
  float CapsuleRadius,
  float CapsuleHalfHeight
)
{
  check(
    bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale
    || !bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup
  )
  check(ShapeBodySetup->AggGeom.SphylElems.Num() == 1)
  FKSphylElem* SE = ShapeBodySetup->AggGeom.SphylElems.GetData();
  const float Length = 2 * FMath::Max(CapsuleHalfHeight - CapsuleRadius, 0.f);
  if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
  {
    SE->SetTransform(FTransform(CapsuleRotation));
    SE->Radius = CapsuleRadius;
    SE->Length = Length;
  }
  else
  {
    if (SE->Rotation != CapsuleRotation || SE->Radius != CapsuleRadius || SE->Length != Length)
    {
      ShapeBodySetup = nullptr;
      bUseArchetypeBodySetup = false;
    }
  }
  return bUseArchetypeBodySetup;
}

template<typename ShapeElemType>
void UGenCapsuleComponent::CreateShapeBodySetupIfNeeded()
{
  if (ShapeBodySetup == nullptr || ShapeBodySetup->IsPendingKill())
  {
    ShapeBodySetup = NewObject<UBodySetup>(this, NAME_None, RF_Transient);
    if (GUObjectArray.IsDisregardForGC(this))
    {
      ShapeBodySetup->AddToRoot();
    }
    ShapeBodySetup->AddToCluster(this);
    if (ShapeBodySetup->HasAnyInternalFlags(EInternalObjectFlags::Async) && GUObjectClusters.GetObjectCluster(ShapeBodySetup))
    {
      ShapeBodySetup->ClearInternalFlags(EInternalObjectFlags::Async);
    }
    ShapeBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
    AddShapeToGeomArray<ShapeElemType>();
    ShapeBodySetup->bNeverNeedsCookedCollisionData = true;
    bUseArchetypeBodySetup = false;
    BodyInstance.BodySetup = ShapeBodySetup;
    {
      if (BodyInstance.IsValidBodyInstance())
      {
        FPhysicsCommand::ExecuteWrite(
          BodyInstance.GetActorReferenceWithWelding(),
          [this](const FPhysicsActorHandle& Actor)
          {
            TArray<FPhysicsShapeHandle> Shapes;
            BodyInstance.GetAllShapes_AssumesLocked(Shapes);
            for (FPhysicsShapeHandle& Shape : Shapes)
            {
              if (BodyInstance.IsShapeBoundToBody(Shape)) SetShapeToNewGeom<ShapeElemType>(Shape);
            }
          }
        );
      }
    }
  }
}

template<>
void UGenCapsuleComponent::AddShapeToGeomArray<FKSphylElem>()
{
  ShapeBodySetup->AggGeom.SphylElems.Add(FKSphylElem());
}

template<>
void UGenCapsuleComponent::SetShapeToNewGeom<FKSphylElem>(const FPhysicsShapeHandle& Shape)
{
  FPhysicsInterface::SetUserData(Shape, (void*)ShapeBodySetup->AggGeom.SphylElems[0].GetUserData());
}
