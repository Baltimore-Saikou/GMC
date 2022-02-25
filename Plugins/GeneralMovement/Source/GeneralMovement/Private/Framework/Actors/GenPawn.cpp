// Copyright 2022 Dominik Scherer. All Rights Reserved.

#include "GenPawn.h"
#define GMC_PAWN_LOG
#include "GMC_LOG.h"

DEFINE_LOG_CATEGORY(LogGMCPawn)

AGenPawn::AGenPawn()
{
  PrimaryActorTick.bCanEverTick = true;
  bAllowTickBeforeBeginPlay = false;
  SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
  BaseEyeHeight = 75.f;
  bReplicates = true;
  NetUpdateFrequency = 60.f;

  // Default actor movement replication must be disabled to use the replication component.
  SetReplicateMovement(false);

  // When true, this causes pawns that were already placed in the world during map load not to be destroyed when they are net culled. This
  // is undesirable for remotely controlled pawns as they will be "stuck" in their last smoothing state until they become relevant again.
  bNetLoadOnClient = false;
}

bool AGenPawn::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
  CA_SUPPRESS(6011);
  if (
    bAlwaysRelevant
    || RealViewer == Controller
    || IsOwnedBy(ViewTarget)
    || IsOwnedBy(RealViewer)
    || this == ViewTarget
    || ViewTarget == GetInstigator()
    || IsBasedOnActor(ViewTarget)
    || (ViewTarget && ViewTarget->IsBasedOnActor(this))
  )
  {
    return true;
  }
  else if ((IsHidden() || bOnlyRelevantToOwner) && (!GetRootComponent() || !GetRootComponent()->IsCollisionEnabled()))
  {
    return false;
  }
  else
  {
    // @attention The super implementation uses the net relevancy of the movement base here. We do not do this as the default movement
    // implementation does not attach the pawn to the movement base.
  }

  return !GetDefault<AGameNetworkManager>()->bUseDistanceBasedRelevancy || IsWithinNetRelevancyDistance(SrcLocation);
}

#if WITH_EDITOR

void AGenPawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

void AGenPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);
  check(PlayerInputComponent);

  BindDirectionalInput(PlayerInputComponent);
  BindViewInput(PlayerInputComponent);
  BindKeyInput(PlayerInputComponent);
}

void AGenPawn::BindDirectionalInput(UInputComponent* PlayerInputComponent)
{
  TArray<FName> Mappings;
  UInputSettings::GetInputSettings()->GetAxisNames(Mappings);
  if (Mappings.Contains(ID_MoveForward)) PlayerInputComponent->BindAxis(ID_MoveForward, this, &AGenPawn::MoveForward);
  if (Mappings.Contains(ID_MoveRight))   PlayerInputComponent->BindAxis(ID_MoveRight,   this, &AGenPawn::MoveRight);
  if (Mappings.Contains(ID_MoveUp))      PlayerInputComponent->BindAxis(ID_MoveUp,      this, &AGenPawn::MoveUp);
}

void AGenPawn::BindViewInput(UInputComponent* PlayerInputComponent)
{
  TArray<FName> Mappings;
  UInputSettings::GetInputSettings()->GetAxisNames(Mappings);
  if (Mappings.Contains(ID_RollView))      PlayerInputComponent->BindAxis(ID_RollView,      this, &AGenPawn::RollView);
  if (Mappings.Contains(ID_PitchView))     PlayerInputComponent->BindAxis(ID_PitchView,     this, &AGenPawn::PitchView);
  if (Mappings.Contains(ID_TurnView))      PlayerInputComponent->BindAxis(ID_TurnView,      this, &AGenPawn::TurnView);
  if (Mappings.Contains(ID_RollViewRate))  PlayerInputComponent->BindAxis(ID_RollViewRate,  this, &AGenPawn::RollViewRate);
  if (Mappings.Contains(ID_PitchViewRate)) PlayerInputComponent->BindAxis(ID_PitchViewRate, this, &AGenPawn::PitchViewRate);
  if (Mappings.Contains(ID_TurnViewRate))  PlayerInputComponent->BindAxis(ID_TurnViewRate,  this, &AGenPawn::TurnViewRate);
}

void AGenPawn::BindKeyInput(UInputComponent* PlayerInputComponent)
{
  TArray<FName> Mappings;
  UInputSettings::GetInputSettings()->GetActionNames(Mappings);
  if (Mappings.Contains(ID_Action1))  { PlayerInputComponent->BindAction(ID_Action1,  IE_Pressed,  this, &AGenPawn::StartAction1);
                                        PlayerInputComponent->BindAction(ID_Action1,  IE_Released, this, &AGenPawn::StopAction1);   }
  if (Mappings.Contains(ID_Action2))  { PlayerInputComponent->BindAction(ID_Action2,  IE_Pressed,  this, &AGenPawn::StartAction2);
                                        PlayerInputComponent->BindAction(ID_Action2,  IE_Released, this, &AGenPawn::StopAction2);   }
  if (Mappings.Contains(ID_Action3))  { PlayerInputComponent->BindAction(ID_Action3,  IE_Pressed,  this, &AGenPawn::StartAction3);
                                        PlayerInputComponent->BindAction(ID_Action3,  IE_Released, this, &AGenPawn::StopAction3);   }
  if (Mappings.Contains(ID_Action4))  { PlayerInputComponent->BindAction(ID_Action4,  IE_Pressed,  this, &AGenPawn::StartAction4);
                                        PlayerInputComponent->BindAction(ID_Action4,  IE_Released, this, &AGenPawn::StopAction4);   }
  if (Mappings.Contains(ID_Action5))  { PlayerInputComponent->BindAction(ID_Action5,  IE_Pressed,  this, &AGenPawn::StartAction5);
                                        PlayerInputComponent->BindAction(ID_Action5,  IE_Released, this, &AGenPawn::StopAction5);   }
  if (Mappings.Contains(ID_Action6))  { PlayerInputComponent->BindAction(ID_Action6,  IE_Pressed,  this, &AGenPawn::StartAction6);
                                        PlayerInputComponent->BindAction(ID_Action6,  IE_Released, this, &AGenPawn::StopAction6);   }
  if (Mappings.Contains(ID_Action7))  { PlayerInputComponent->BindAction(ID_Action7,  IE_Pressed,  this, &AGenPawn::StartAction7);
                                        PlayerInputComponent->BindAction(ID_Action7,  IE_Released, this, &AGenPawn::StopAction7);   }
  if (Mappings.Contains(ID_Action8))  { PlayerInputComponent->BindAction(ID_Action8,  IE_Pressed,  this, &AGenPawn::StartAction8);
                                        PlayerInputComponent->BindAction(ID_Action8,  IE_Released, this, &AGenPawn::StopAction8);   }
  if (Mappings.Contains(ID_Action9))  { PlayerInputComponent->BindAction(ID_Action9,  IE_Pressed,  this, &AGenPawn::StartAction9);
                                        PlayerInputComponent->BindAction(ID_Action9,  IE_Released, this, &AGenPawn::StopAction9);   }
  if (Mappings.Contains(ID_Action10)) { PlayerInputComponent->BindAction(ID_Action10, IE_Pressed,  this, &AGenPawn::StartAction10);
                                        PlayerInputComponent->BindAction(ID_Action10, IE_Released, this, &AGenPawn::StopAction10);  }
  if (Mappings.Contains(ID_Action11)) { PlayerInputComponent->BindAction(ID_Action11, IE_Pressed,  this, &AGenPawn::StartAction11);
                                        PlayerInputComponent->BindAction(ID_Action11, IE_Released, this, &AGenPawn::StopAction11);  }
  if (Mappings.Contains(ID_Action12)) { PlayerInputComponent->BindAction(ID_Action12, IE_Pressed,  this, &AGenPawn::StartAction12);
                                        PlayerInputComponent->BindAction(ID_Action12, IE_Released, this, &AGenPawn::StopAction12);  }
  if (Mappings.Contains(ID_Action13)) { PlayerInputComponent->BindAction(ID_Action13, IE_Pressed,  this, &AGenPawn::StartAction13);
                                        PlayerInputComponent->BindAction(ID_Action13, IE_Released, this, &AGenPawn::StopAction13);  }
  if (Mappings.Contains(ID_Action14)) { PlayerInputComponent->BindAction(ID_Action14, IE_Pressed,  this, &AGenPawn::StartAction14);
                                        PlayerInputComponent->BindAction(ID_Action14, IE_Released, this, &AGenPawn::StopAction14);  }
  if (Mappings.Contains(ID_Action15)) { PlayerInputComponent->BindAction(ID_Action15, IE_Pressed,  this, &AGenPawn::StartAction15);
                                        PlayerInputComponent->BindAction(ID_Action15, IE_Released, this, &AGenPawn::StopAction15);  }
  if (Mappings.Contains(ID_Action16)) { PlayerInputComponent->BindAction(ID_Action16, IE_Pressed,  this, &AGenPawn::StartAction16);
                                        PlayerInputComponent->BindAction(ID_Action16, IE_Released, this, &AGenPawn::StopAction16);  }
}

void AGenPawn::MoveForward(float Scale)
{
  if (Scale != 0.f)
  {
    if (Controller)
    {
      if (InputMode == EInputMode::None) return;
      if (InputMode == EInputMode::AllAbsolute)
      {
        AddMovementInput(FVector(Scale, 0.f, 0.f), 1.f);
        return;
      }
      FRotator ControlRotation = Controller->GetControlRotation();
      if (InputMode == EInputMode::AbsoluteZ) ControlRotation.Pitch = 0.f;
      FVector WorldControlDirection = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::X);
      if (InputMode == EInputMode::AbsoluteZ) WorldControlDirection.Z = 0.f;
      AddMovementInput(WorldControlDirection, Scale);
    }
  }
}

void AGenPawn::MoveRight(float Scale)
{
  if (Scale != 0.f)
  {
    if (Controller)
    {
      if (InputMode == EInputMode::None) return;
      if (InputMode == EInputMode::AllAbsolute)
      {
        AddMovementInput(FVector(0.f, Scale, 0.f), 1.f);
        return;
      }
      FRotator ControlRotation = Controller->GetControlRotation();
      if (InputMode == EInputMode::AbsoluteZ) ControlRotation.Pitch = 0.f;
      FVector WorldControlDirection = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);
      if (InputMode == EInputMode::AbsoluteZ) WorldControlDirection.Z = 0.f;
      AddMovementInput(WorldControlDirection, Scale);
    }
  }
}

void AGenPawn::MoveUp(float Scale)
{
  if (Scale != 0.f)
  {
    if (Controller)
    {
      if (InputMode == EInputMode::None) return;
      if (InputMode == EInputMode::AllAbsolute || InputMode == EInputMode::AbsoluteZ)
      {
        AddMovementInput(FVector(0.f, 0.f, Scale), 1.f);
        return;
      }
      const FRotator ControlRotation = Controller->GetControlRotation();
      const FVector WorldControlDirection = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Z);
      AddMovementInput(WorldControlDirection, Scale);
    }
  }
}

void AGenPawn::RollView(float Value)
{
  AddControllerRollInput(Value * GetMouseSensitivityRoll() * (bInvertRollView ? -1.f : 1.f));
}

void AGenPawn::PitchView(float Value)
{
  AddControllerPitchInput(Value * GetMouseSensitivityPitch() * (bInvertPitchView ? -1.f : 1.f));
}

void AGenPawn::TurnView(float Value)
{
  AddControllerYawInput(Value * GetMouseSensitivityTurn() * (bInvertTurnView ? -1.f : 1.f));
}

void AGenPawn::RollViewRate(float Value)
{
  if (const auto World = GetWorld())
  {
    AddControllerRollInput(Value * GetAnalogSensitivityRoll() * World->GetDeltaSeconds() * (bInvertRollView ? -1.f : 1.f));
  }
}

void AGenPawn::PitchViewRate(float Value)
{
  if (const auto World = GetWorld())
  {
    AddControllerPitchInput(Value * GetAnalogSensitivityPitch() * World->GetDeltaSeconds() * (bInvertPitchView ? -1.f : 1.f));
  }
}

void AGenPawn::TurnViewRate(float Value)
{
  if (const auto World = GetWorld())
  {
    AddControllerYawInput(Value * GetAnalogSensitivityTurn() * World->GetDeltaSeconds() * (bInvertTurnView ? -1.f : 1.f));
  }
}

void AGenPawn::StartAction1()
{
  Flag1_bActive = true;
}

void AGenPawn::StopAction1()
{
  Flag1_bActive = false;
}

void AGenPawn::StartAction2()
{
  Flag2_bActive = true;
}

void AGenPawn::StopAction2()
{
  Flag2_bActive = false;
}

void AGenPawn::StartAction3()
{
  Flag3_bActive = true;
}

void AGenPawn::StopAction3()
{
  Flag3_bActive = false;
}

void AGenPawn::StartAction4()
{
  Flag4_bActive = true;
}

void AGenPawn::StopAction4()
{
  Flag4_bActive = false;
}

void AGenPawn::StartAction5()
{
  Flag5_bActive = true;
}

void AGenPawn::StopAction5()
{
  Flag5_bActive = false;
}

void AGenPawn::StartAction6()
{
  Flag6_bActive = true;
}

void AGenPawn::StopAction6()
{
  Flag6_bActive = false;
}

void AGenPawn::StartAction7()
{
  Flag7_bActive = true;
}

void AGenPawn::StopAction7()
{
  Flag7_bActive = false;
}

void AGenPawn::StartAction8()
{
  Flag8_bActive = true;
}

void AGenPawn::StopAction8()
{
  Flag8_bActive = false;
}

void AGenPawn::StartAction9()
{
  Flag9_bActive = true;
}

void AGenPawn::StopAction9()
{
  Flag9_bActive = false;
}

void AGenPawn::StartAction10()
{
  Flag10_bActive = true;
}

void AGenPawn::StopAction10()
{
  Flag10_bActive = false;
}

void AGenPawn::StartAction11()
{
  Flag11_bActive = true;
}

void AGenPawn::StopAction11()
{
  Flag11_bActive = false;
}

void AGenPawn::StartAction12()
{
  Flag12_bActive = true;
}

void AGenPawn::StopAction12()
{
  Flag12_bActive = false;
}

void AGenPawn::StartAction13()
{
  Flag13_bActive = true;
}

void AGenPawn::StopAction13()
{
  Flag13_bActive = false;
}

void AGenPawn::StartAction14()
{
  Flag14_bActive = true;
}

void AGenPawn::StopAction14()
{
  Flag14_bActive = false;
}

void AGenPawn::StartAction15()
{
  Flag15_bActive = true;
}

void AGenPawn::StopAction15()
{
  Flag15_bActive = false;
}

void AGenPawn::StartAction16()
{
  Flag16_bActive = true;
}

void AGenPawn::StopAction16()
{
  Flag16_bActive = false;
}

bool AGenPawn::InputFlag1() const
{
  return Flag1_bActive;
}

bool AGenPawn::InputFlag2() const
{
  return Flag2_bActive;
}

bool AGenPawn::InputFlag3() const
{
  return Flag3_bActive;
}

bool AGenPawn::InputFlag4() const
{
  return Flag4_bActive;
}

bool AGenPawn::InputFlag5() const
{
  return Flag5_bActive;
}

bool AGenPawn::InputFlag6() const
{
  return Flag6_bActive;
}

bool AGenPawn::InputFlag7() const
{
  return Flag7_bActive;
}

bool AGenPawn::InputFlag8() const
{
  return Flag8_bActive;
}

bool AGenPawn::InputFlag9() const
{
  return Flag9_bActive;
}

bool AGenPawn::InputFlag10() const
{
  return Flag10_bActive;
}

bool AGenPawn::InputFlag11() const
{
  return Flag11_bActive;
}

bool AGenPawn::InputFlag12() const
{
  return Flag12_bActive;
}

bool AGenPawn::InputFlag13() const
{
  return Flag13_bActive;
}

bool AGenPawn::InputFlag14() const
{
  return Flag14_bActive;
}

bool AGenPawn::InputFlag15() const
{
  return Flag15_bActive;
}

bool AGenPawn::InputFlag16() const
{
  return Flag16_bActive;
}
