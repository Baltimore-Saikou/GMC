// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "GenPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGMCPawn, Log, All);

UENUM(BlueprintType)
enum class EInputMode : uint8
{
  None UMETA(DisplayName = "None", ToolTip = "Directional input is disabled."),
  AllRelative UMETA(DisplayName = "AllRelative", ToolTip = "Input is all relative to the controller view meaning forward is whatever direction we are currently looking towards. This mode is most commonly used for three-dimensional movement, e.g. flying or swimming."),
  AbsoluteZ UMETA(DisplayName = "AbsoluteZ", ToolTip = "Only the Z component of the input vector is absolute, X and Y components are relative to the controller view, but with zeroed pitch i.e. forward is affected only by the view yaw. This is the standard input mode you would expect for a game that lets you control a character that is walking on the ground."),
  AllAbsolute UMETA(DisplayName = "AllAbsolute", ToolTip = "All components of the input vector are absolute and unaffected by the controller view. This is most commonly used for vehicular input where the input values usually represent the state of something more abstract like the gas pedal or the steering wheel."),
  MAX UMETA(Hidden)
};

/// Pawn class intended to be used with @see UGenMovementReplicationComponent.
UCLASS(BlueprintType, Blueprintable)
class GMC_API AGenPawn : public APawn
{
  GENERATED_BODY()

public:

  AGenPawn();
  bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

#if WITH_EDITOR

  void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetInputMode(EInputMode NewInputMode);

  UFUNCTION(BlueprintCallable, Category = "Input")
  EInputMode GetInputMode() const;

  UFUNCTION(BlueprintCallable, Category = "Input")
  float GetMouseSensitivityRoll() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetMouseSensitivityRoll(float NewValue);
  UFUNCTION(BlueprintCallable, Category = "Input")
  float GetMouseSensitivityPitch() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetMouseSensitivityPitch(float NewValue);
  UFUNCTION(BlueprintCallable, Category = "Input")
  float GetMouseSensitivityTurn() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetMouseSensitivityTurn(float NewValue);
  UFUNCTION(BlueprintCallable, Category = "Input")
  float GetAnalogSensitivityRoll() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetAnalogSensitivityRoll(float NewValue);
  UFUNCTION(BlueprintCallable, Category = "Input")
  float GetAnalogSensitivityPitch() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetAnalogSensitivityPitch(float NewValue);
  UFUNCTION(BlueprintCallable, Category = "Input")
  float GetAnalogSensitivityTurn() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetAnalogSensitivityTurn(float NewValue);
  UFUNCTION(BlueprintCallable, Category = "Input")
  bool IsRollViewInverted() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetInvertRollView(bool bInvert);
  UFUNCTION(BlueprintCallable, Category = "Input")
  bool IsPitchViewInverted() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetInvertPitchView(bool bInvert);
  UFUNCTION(BlueprintCallable, Category = "Input")
  bool IsTurnViewInverted() const;
  UFUNCTION(BlueprintCallable, Category = "Input")
  void SetInvertTurnView(bool bInvert);

  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void MoveForward(float Scale);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void MoveRight(float Scale);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void MoveUp(float Scale);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void RollView(float Value);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void PitchView(float Value);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void TurnView(float Value);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void RollViewRate(float Value);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void PitchViewRate(float Value);
  UFUNCTION(BlueprintCallable, Category = "Input|Axis Mappings")
  virtual void TurnViewRate(float Value);

  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction1();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction1();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction2();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction2();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction3();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction3();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction4();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction4();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction5();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction5();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction6();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction6();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction7();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction7();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction8();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction8();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction9();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction9();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction10();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction10();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction11();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction11();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction12();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction12();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction13();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction13();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction14();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction14();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction15();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction15();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StartAction16();
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  virtual void StopAction16();

  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag1() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag2() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag3() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag4() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag5() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag6() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag7() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag8() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag9() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag10() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag11() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag12() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag13() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag14() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag15() const;
  UFUNCTION(BlueprintCallable, Category = "Input|Action Mappings")
  bool InputFlag16() const;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_MoveForward{"Move Forward"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_MoveRight{"Move Right"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_MoveUp{"Move Up"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_RollView{"Roll View"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_PitchView{"Pitch View"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_TurnView{"Turn View"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_RollViewRate{"Roll View Rate"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_PitchViewRate{"Pitch View Rate"};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Axis Mappings")
  FName ID_TurnViewRate{"Turn View Rate"};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action1{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action2{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action3{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action4{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action5{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action6{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action7{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action8{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action9{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action10{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action11{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action12{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action13{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action14{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action15{""};
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Action Mappings")
  FName ID_Action16{""};

protected:

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (UIMin = "0.01", UIMax = "10"))
  /// Input sensitivity for rolling the view with the mouse.
  float MouseSensitivityRoll{1.f};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (UIMin = "0.01", UIMax = "10"))
  /// Input sensitivity for pitching the view with the mouse.
  float MouseSensitivityPitch{1.f};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (UIMin = "0.01", UIMax = "10"))
  /// Input sensitivity for turning the view with the mouse.
  float MouseSensitivityTurn{1.f};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (UIMin = "20", UIMax = "180"))
  /// Rate for rolling the view with analog input (deg/s).
  float AnalogSensitivityRoll{90.f};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (UIMin = "20", UIMax = "180"))
  /// Rate for pitching the view with analog input (deg/s).
  float AnalogSensitivityPitch{90.f};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (UIMin = "20", UIMax = "180"))
  /// Rate for turning the view with analog input (deg/s).
  float AnalogSensitivityTurn{90.f};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
  /// Whether roll view input should be inverted.
  bool bInvertRollView{false};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
  /// Whether pitch view input should be inverted.
  bool bInvertPitchView{false};
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
  /// Whether turn view input should be inverted.
  bool bInvertTurnView{false};

  void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  virtual void BindDirectionalInput(UInputComponent* PlayerInputComponent);
  virtual void BindViewInput(UInputComponent* PlayerInputComponent);
  virtual void BindKeyInput(UInputComponent* PlayerInputComponent);

private:

  /// The input mode determines how the player's directional input is interpreted.
  EInputMode InputMode{EInputMode::AbsoluteZ};

  bool Flag1_bActive{false};
  bool Flag2_bActive{false};
  bool Flag3_bActive{false};
  bool Flag4_bActive{false};
  bool Flag5_bActive{false};
  bool Flag6_bActive{false};
  bool Flag7_bActive{false};
  bool Flag8_bActive{false};
  bool Flag9_bActive{false};
  bool Flag10_bActive{false};
  bool Flag11_bActive{false};
  bool Flag12_bActive{false};
  bool Flag13_bActive{false};
  bool Flag14_bActive{false};
  bool Flag15_bActive{false};
  bool Flag16_bActive{false};
};

FORCEINLINE void AGenPawn::SetInputMode(EInputMode NewInputMode)
{
  InputMode = NewInputMode;
}

FORCEINLINE EInputMode AGenPawn::GetInputMode() const
{
  return InputMode;
}

FORCEINLINE float AGenPawn::GetMouseSensitivityRoll() const
{
  return MouseSensitivityRoll;
}

FORCEINLINE void AGenPawn::SetMouseSensitivityRoll(float NewValue)
{
  MouseSensitivityRoll = NewValue;
}

FORCEINLINE float AGenPawn::GetMouseSensitivityPitch() const
{
  return MouseSensitivityPitch;
}

FORCEINLINE void AGenPawn::SetMouseSensitivityPitch(float NewValue)
{
  MouseSensitivityPitch = NewValue;
}

FORCEINLINE float AGenPawn::GetMouseSensitivityTurn() const
{
  return MouseSensitivityTurn;
}

FORCEINLINE void AGenPawn::SetMouseSensitivityTurn(float NewValue)
{
  MouseSensitivityTurn = NewValue;
}

FORCEINLINE float AGenPawn::GetAnalogSensitivityRoll() const
{
  return AnalogSensitivityRoll;
}

FORCEINLINE void AGenPawn::SetAnalogSensitivityRoll(float NewValue)
{
  AnalogSensitivityRoll = NewValue;
}

FORCEINLINE float AGenPawn::GetAnalogSensitivityPitch() const
{
  return AnalogSensitivityPitch;
}

FORCEINLINE void AGenPawn::SetAnalogSensitivityPitch(float NewValue)
{
  AnalogSensitivityPitch = NewValue;
}

FORCEINLINE float AGenPawn::GetAnalogSensitivityTurn() const
{
  return AnalogSensitivityTurn;
}

FORCEINLINE void AGenPawn::SetAnalogSensitivityTurn(float NewValue)
{
  AnalogSensitivityTurn = NewValue;
}

FORCEINLINE bool AGenPawn::IsRollViewInverted() const
{
  return bInvertRollView;
}

FORCEINLINE void AGenPawn::SetInvertRollView(bool bInvert)
{
  bInvertRollView = bInvert;
}

FORCEINLINE bool AGenPawn::IsPitchViewInverted() const
{
  return bInvertPitchView;
}

FORCEINLINE void AGenPawn::SetInvertPitchView(bool bInvert)
{
  bInvertPitchView = bInvert;
}

FORCEINLINE bool AGenPawn::IsTurnViewInverted() const
{
  return bInvertTurnView;
}

FORCEINLINE void AGenPawn::SetInvertTurnView(bool bInvert)
{
  bInvertTurnView = bInvert;
}
