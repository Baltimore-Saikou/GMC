// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

//------------------------------------------------------------------------------------------------------------------------------------------
// Extendable Replication System

// @attention To expose newly added data members to Blueprint, Blueprint-callable UFUNCTIONs must be provided for binding which just need
// to call the generated C++ implementations Bind##Name, Bind##Name##WithAccessor and GetPreReplicated##Name##FromState. For the latter a
// Blueprint-type UENUM which enumerates the added data members must be provided as well.
// @attention It is possible to provide your own implementations for validating and net serializing data members by adding the macros
// CALL_IsBoundDataValidSpecific(Name) and CALL_SerializeBoundDataSpecific(Name) respectively instead of the generic variants of the macros
// (@see IsBoundDataValid_IMPLEMENTATION, @see SerializeBoundData_IMPLEMENTATION).
// @attention You can add user-defined UENUM classes (not un-scoped or non-reflected enums) but the definition should be placed within the
// GenMovementReplicationComponent header, otherwise the UHT will might get confused (it is not advisable to include child class headers).

// Adds data members to FMove. Added types must support the == and != operators.
#define DEFINE_PREREPLICATED_DATA_FMOVE()\
  ADD_PREREPLICATED_TYPE_TO_MOVE(bool,             Bool)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(uint8,            HalfByte)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(uint8,            Byte)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(int32,            Int)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(float,            Float)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(FVector,          Vector)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(FVector,          Normal)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(FRotator,         Rotator)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(AActor*,          ActorReference)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(UActorComponent*, ActorComponentReference)\
  ADD_PREREPLICATED_TYPE_TO_MOVE(UAnimMontage*,    AnimMontageReference)

// Adds data members to FState. Added types must support the == and != operator.
#define DEFINE_PREREPLICATED_DATA_FSTATE()\
  ADD_PREREPLICATED_TYPE_TO_STATE(bool,             Bool)\
  ADD_PREREPLICATED_TYPE_TO_STATE(uint8,            HalfByte)\
  ADD_PREREPLICATED_TYPE_TO_STATE(uint8,            Byte)\
  ADD_PREREPLICATED_TYPE_TO_STATE(int32,            Int)\
  ADD_PREREPLICATED_TYPE_TO_STATE(float,            Float)\
  ADD_PREREPLICATED_TYPE_TO_STATE(FVector,          Vector)\
  ADD_PREREPLICATED_TYPE_TO_STATE(FVector,          Normal)\
  ADD_PREREPLICATED_TYPE_TO_STATE(FRotator,         Rotator)\
  ADD_PREREPLICATED_TYPE_TO_STATE(AActor*,          ActorReference)\
  ADD_PREREPLICATED_TYPE_TO_STATE(UActorComponent*, ActorComponentReference)\
  ADD_PREREPLICATED_TYPE_TO_STATE(UAnimMontage*,    AnimMontageReference)

// Adds data members to FState. Added types must support the == and != operator.
#define DEFINE_PREREPLICATED_DATA_FSTATEREDUCED()\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(bool,             Bool)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(uint8,            HalfByte)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(uint8,            Byte)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(int32,            Int)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(float,            Float)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(FVector,          Vector)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(FVector,          Normal)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(FRotator,         Rotator)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(AActor*,          ActorReference)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(UActorComponent*, ActorComponentReference)\
  ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(UAnimMontage*,    AnimMontageReference)

// Implements the required functions and references within UGenMovementReplicationComponent for replication of the added data members.
#define IMPLEMENT_REPLICATION_SYSTEM()\
  ADD_REPLICATION_LOGIC(bool,             Bool)\
  ADD_REPLICATION_LOGIC(uint8,            HalfByte)\
  ADD_REPLICATION_LOGIC(uint8,            Byte)\
  ADD_REPLICATION_LOGIC(int32,            Int)\
  ADD_REPLICATION_LOGIC(float,            Float)\
  ADD_REPLICATION_LOGIC(FVector,          Vector)\
  ADD_REPLICATION_LOGIC(FVector,          Normal)\
  ADD_REPLICATION_LOGIC(FRotator,         Rotator)\
  ADD_REPLICATION_LOGIC(AActor*,          ActorReference)\
  ADD_REPLICATION_LOGIC(UActorComponent*, ActorComponentReference)\
  ADD_REPLICATION_LOGIC(UAnimMontage*,    AnimMontageReference)

// Swaps the state of smoothed server pawns between "interpolated" for local visuals and "non-interpolated" for client move simulation.
#define SwapStateBufferBoundData_IMPLEMENTATION()\
  CALL_SwapStateBufferBoundData(Bool)\
  CALL_SwapStateBufferBoundData(HalfByte)\
  CALL_SwapStateBufferBoundData(Byte)\
  CALL_SwapStateBufferBoundData(Int)\
  CALL_SwapStateBufferBoundData(Float)\
  CALL_SwapStateBufferBoundData(Vector)\
  CALL_SwapStateBufferBoundData(Normal)\
  CALL_SwapStateBufferBoundData(Rotator)\
  CALL_SwapStateBufferBoundData(ActorReference)\
  CALL_SwapStateBufferBoundData(ActorComponentReference)\
  CALL_SwapStateBufferBoundData(AnimMontageReference)

// Implements the checks for forcing a replication update to the client when a data member changes.
#define ForceNetUpdateCheckBoundData_IMPLEMENTATION()\
  CALL_ForceNetUpdateCheckBoundData(Bool)\
  CALL_ForceNetUpdateCheckBoundData(HalfByte)\
  CALL_ForceNetUpdateCheckBoundData(Byte)\
  CALL_ForceNetUpdateCheckBoundData(Int)\
  CALL_ForceNetUpdateCheckBoundData(Float)\
  CALL_ForceNetUpdateCheckBoundData(Vector)\
  CALL_ForceNetUpdateCheckBoundData(Normal)\
  CALL_ForceNetUpdateCheckBoundData(Rotator)\
  CALL_ForceNetUpdateCheckBoundData(ActorReference)\
  CALL_ForceNetUpdateCheckBoundData(ActorComponentReference)\
  CALL_ForceNetUpdateCheckBoundData(AnimMontageReference)

// Saves the data members into the server state for replication to the client.
#define SaveBoundDataToServerState_IMPLEMENTATION()\
  CALL_SaveBoundDataToServerState(Bool)\
  CALL_SaveBoundDataToServerState(HalfByte)\
  CALL_SaveBoundDataToServerState(Byte)\
  CALL_SaveBoundDataToServerState(Int)\
  CALL_SaveBoundDataToServerState(Float)\
  CALL_SaveBoundDataToServerState(Vector)\
  CALL_SaveBoundDataToServerState(Normal)\
  CALL_SaveBoundDataToServerState(Rotator)\
  CALL_SaveBoundDataToServerState(ActorReference)\
  CALL_SaveBoundDataToServerState(ActorComponentReference)\
  CALL_SaveBoundDataToServerState(AnimMontageReference)

// Resets the locks for saving data to the simulated proxy server state.
#define ResetLockedBoundData_IMPLEMENTATION()\
  CALL_ResetLockedBoundData(Bool)\
  CALL_ResetLockedBoundData(HalfByte)\
  CALL_ResetLockedBoundData(Byte)\
  CALL_ResetLockedBoundData(Int)\
  CALL_ResetLockedBoundData(Float)\
  CALL_ResetLockedBoundData(Vector)\
  CALL_ResetLockedBoundData(Normal)\
  CALL_ResetLockedBoundData(Rotator)\
  CALL_ResetLockedBoundData(ActorReference)\
  CALL_ResetLockedBoundData(ActorComponentReference)\
  CALL_ResetLockedBoundData(AnimMontageReference)

// Unpacks the bound data of a server state when the client receives a replication update.
#define UnpackBoundData_IMPLEMENTATION()\
  /*Bool data is always (de)serialized directly per bit and doesn't require unpacking.*/\
  CALL_UnpackBoundData(HalfByte)\
  CALL_UnpackBoundData(Byte)\
  CALL_UnpackBoundData(Int)\
  CALL_UnpackBoundData(Float)\
  CALL_UnpackBoundData(Vector)\
  CALL_UnpackBoundData(Normal)\
  CALL_UnpackBoundData(Rotator)\
  CALL_UnpackBoundData(ActorReference)\
  CALL_UnpackBoundData(ActorComponentReference)\
  CALL_UnpackBoundData(AnimMontageReference)

// Adopts the bound data from the autonomous proxy server state for replay, replacing non-replicated values with those from the source move.
#define LoadBoundDataForReplay_IMPLEMENTATION()\
  CALL_LoadBoundDataForReplay(Bool)\
  CALL_LoadBoundDataForReplay(HalfByte)\
  CALL_LoadBoundDataForReplay(Byte)\
  CALL_LoadBoundDataForReplay(Int)\
  CALL_LoadBoundDataForReplay(Float)\
  CALL_LoadBoundDataForReplay(Vector)\
  CALL_LoadBoundDataForReplay(Normal)\
  CALL_LoadBoundDataForReplay(Rotator)\
  CALL_LoadBoundDataForReplay(ActorReference)\
  CALL_LoadBoundDataForReplay(ActorComponentReference)\
  CALL_LoadBoundDataForReplay(AnimMontageReference)

// Saves bound data values into the input data members of a move. This is usually done with the local client move to support move combining
// and replays.
#define SaveInBoundDataToMove_IMPLEMENTATION()\
  CALL_SaveInBoundDataToMove(Bool)\
  CALL_SaveInBoundDataToMove(HalfByte)\
  CALL_SaveInBoundDataToMove(Byte)\
  CALL_SaveInBoundDataToMove(Int)\
  CALL_SaveInBoundDataToMove(Float)\
  CALL_SaveInBoundDataToMove(Vector)\
  CALL_SaveInBoundDataToMove(Normal)\
  CALL_SaveInBoundDataToMove(Rotator)\
  CALL_SaveInBoundDataToMove(ActorReference)\
  CALL_SaveInBoundDataToMove(ActorComponentReference)\
  CALL_SaveInBoundDataToMove(AnimMontageReference)

// Saves bound data values into the output data members of a move. This is usually done with the local client move to support client side
// verification of bound data.
#define SaveOutBoundDataToMove_IMPLEMENTATION()\
  CALL_SaveOutBoundDataToMove(Bool)\
  CALL_SaveOutBoundDataToMove(HalfByte)\
  CALL_SaveOutBoundDataToMove(Byte)\
  CALL_SaveOutBoundDataToMove(Int)\
  CALL_SaveOutBoundDataToMove(Float)\
  CALL_SaveOutBoundDataToMove(Vector)\
  CALL_SaveOutBoundDataToMove(Normal)\
  CALL_SaveOutBoundDataToMove(Rotator)\
  CALL_SaveOutBoundDataToMove(ActorReference)\
  CALL_SaveOutBoundDataToMove(ActorComponentReference)\
  CALL_SaveOutBoundDataToMove(AnimMontageReference)

// Saves bound data values into the appropriate data members of a state.
#define SaveBoundDataToState_IMPLEMENTATION()\
  CALL_SaveBoundDataToState(Bool)\
  CALL_SaveBoundDataToState(HalfByte)\
  CALL_SaveBoundDataToState(Byte)\
  CALL_SaveBoundDataToState(Int)\
  CALL_SaveBoundDataToState(Float)\
  CALL_SaveBoundDataToState(Vector)\
  CALL_SaveBoundDataToState(Normal)\
  CALL_SaveBoundDataToState(Rotator)\
  CALL_SaveBoundDataToState(ActorReference)\
  CALL_SaveBoundDataToState(ActorComponentReference)\
  CALL_SaveBoundDataToState(AnimMontageReference)

// Loads the input data members from a move. This is usually done at the beginning of a move execution.
#define LoadInBoundDataFromMove_IMPLEMENTATION()\
  CALL_LoadInBoundDataFromMove(Bool)\
  CALL_LoadInBoundDataFromMove(HalfByte)\
  CALL_LoadInBoundDataFromMove(Byte)\
  CALL_LoadInBoundDataFromMove(Int)\
  CALL_LoadInBoundDataFromMove(Float)\
  CALL_LoadInBoundDataFromMove(Vector)\
  CALL_LoadInBoundDataFromMove(Normal)\
  CALL_LoadInBoundDataFromMove(Rotator)\
  CALL_LoadInBoundDataFromMove(ActorReference)\
  CALL_LoadInBoundDataFromMove(ActorComponentReference)\
  CALL_LoadInBoundDataFromMove(AnimMontageReference)

// Loads bound data members from a state.
#define LoadBoundDataFromState_IMPLEMENTATION()\
  CALL_LoadBoundDataFromState(Bool)\
  CALL_LoadBoundDataFromState(HalfByte)\
  CALL_LoadBoundDataFromState(Byte)\
  CALL_LoadBoundDataFromState(Int)\
  CALL_LoadBoundDataFromState(Float)\
  CALL_LoadBoundDataFromState(Vector)\
  CALL_LoadBoundDataFromState(Normal)\
  CALL_LoadBoundDataFromState(Rotator)\
  CALL_LoadBoundDataFromState(ActorReference)\
  CALL_LoadBoundDataFromState(ActorComponentReference)\
  CALL_LoadBoundDataFromState(AnimMontageReference)

// Loads bound data members that are replicated to simulated proxies from a state.
#define LoadReplicatedBoundDataFromState_IMPLEMENTATION()\
  CALL_LoadReplicatedBoundDataFromState(Bool)\
  CALL_LoadReplicatedBoundDataFromState(HalfByte)\
  CALL_LoadReplicatedBoundDataFromState(Byte)\
  CALL_LoadReplicatedBoundDataFromState(Int)\
  CALL_LoadReplicatedBoundDataFromState(Float)\
  CALL_LoadReplicatedBoundDataFromState(Vector)\
  CALL_LoadReplicatedBoundDataFromState(Normal)\
  CALL_LoadReplicatedBoundDataFromState(Rotator)\
  CALL_LoadReplicatedBoundDataFromState(ActorReference)\
  CALL_LoadReplicatedBoundDataFromState(ActorComponentReference)\
  CALL_LoadReplicatedBoundDataFromState(AnimMontageReference)

// Adds logic to make smoothing data more accurate.
#define AddTargetStateBoundDataToInitState_IMPLEMENTATION()\
  CALL_AddTargetStateBoundDataToInitState(Bool)\
  CALL_AddTargetStateBoundDataToInitState(HalfByte)\
  CALL_AddTargetStateBoundDataToInitState(Byte)\
  CALL_AddTargetStateBoundDataToInitState(Int)\
  CALL_AddTargetStateBoundDataToInitState(Float)\
  CALL_AddTargetStateBoundDataToInitState(Vector)\
  CALL_AddTargetStateBoundDataToInitState(Normal)\
  CALL_AddTargetStateBoundDataToInitState(Rotator)\
  CALL_AddTargetStateBoundDataToInitState(ActorReference)\
  CALL_AddTargetStateBoundDataToInitState(ActorComponentReference)\
  CALL_AddTargetStateBoundDataToInitState(AnimMontageReference)

// Adds logic to make smoothing data more accurate.
#define AddStartStateBoundDataToInitState_IMPLEMENTATION()\
  CALL_AddStartStateBoundDataToInitState(Bool)\
  CALL_AddStartStateBoundDataToInitState(HalfByte)\
  CALL_AddStartStateBoundDataToInitState(Byte)\
  CALL_AddStartStateBoundDataToInitState(Int)\
  CALL_AddStartStateBoundDataToInitState(Float)\
  CALL_AddStartStateBoundDataToInitState(Vector)\
  CALL_AddStartStateBoundDataToInitState(Normal)\
  CALL_AddStartStateBoundDataToInitState(Rotator)\
  CALL_AddStartStateBoundDataToInitState(ActorReference)\
  CALL_AddStartStateBoundDataToInitState(ActorComponentReference)\
  CALL_AddStartStateBoundDataToInitState(AnimMontageReference)

// Check if the local bound data is valid when compared to the values received from the server. If it isn't, a replay will be triggered.
// Supports type specific implementations. Add CALL_IsBoundDataValidSpecific(Name) and provide the function
//   bool UGenMovementReplicationComponent::Client_IsValid##Name(const FMove& SourceMove) const { ... }
// which must return true if the bound data is valid and false otherwise.
#define IsBoundDataValid_IMPLEMENTATION()\
  CALL_IsBoundDataValidGeneric(Bool)\
  CALL_IsBoundDataValidGeneric(HalfByte)\
  CALL_IsBoundDataValidGeneric(Byte)\
  CALL_IsBoundDataValidGeneric(Int)\
  CALL_IsBoundDataValidSpecific(Float)\
  CALL_IsBoundDataValidSpecific(Vector)\
  CALL_IsBoundDataValidSpecific(Normal)\
  CALL_IsBoundDataValidSpecific(Rotator)\
  CALL_IsBoundDataValidGeneric(ActorReference)\
  CALL_IsBoundDataValidGeneric(ActorComponentReference)\
  CALL_IsBoundDataValidGeneric(AnimMontageReference)

// Implements the net serialization for replicated data members.
// Supports type specific implementations. Add CALL_SerializeBoundDataSpecific(Name) and provide the function
//   void FState::Serialize##Name##Types(FArchive& Ar) { ... }
// which must serialize the bound data to the passed archive.
#define SerializeBoundData_IMPLEMENTATION()\
  CALL_SerializeBoundDataSpecific(Bool)\
  CALL_SerializeBoundDataSpecific(HalfByte)\
  CALL_SerializeBoundDataGeneric(Byte)\
  CALL_SerializeBoundDataGeneric(Int)\
  CALL_SerializeBoundDataGeneric(Float)\
  CALL_SerializeBoundDataSpecific(Vector)\
  CALL_SerializeBoundDataSpecific(Normal)\
  CALL_SerializeBoundDataSpecific(Rotator)\
  CALL_SerializeBoundDataGeneric(ActorReference)\
  CALL_SerializeBoundDataGeneric(ActorComponentReference)\
  CALL_SerializeBoundDataGeneric(AnimMontageReference)

// Blueprint-type UENUMs that enumerate the variable names used to access the pre-replicated data of a state from Blueprint.
UENUM(BlueprintType)
enum class EPreReplicatedBool : uint8
{ Bool1, Bool2, Bool3, Bool4, Bool5, Bool6, Bool7, Bool8, Bool9, Bool10, Bool11, Bool12, Bool13, Bool14, Bool15, Bool16 };
UENUM(BlueprintType)
enum class EPreReplicatedHalfByte : uint8
{ HalfByte1, HalfByte2, HalfByte3, HalfByte4, HalfByte5, HalfByte6, HalfByte7, HalfByte8, HalfByte9, HalfByte10, HalfByte11, HalfByte12, HalfByte13, HalfByte14, HalfByte15, HalfByte16 };
UENUM(BlueprintType)
enum class EPreReplicatedByte : uint8
{ Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7, Byte8, Byte9, Byte10, Byte11, Byte12, Byte13, Byte14, Byte15, Byte16 };
UENUM(BlueprintType)
enum class EPreReplicatedInt : uint8
{ Int1, Int2, Int3, Int4, Int5, Int6, Int7, Int8, Int9, Int10, Int11, Int12, Int13, Int14, Int15, Int16 };
UENUM(BlueprintType)
enum class EPreReplicatedFloat : uint8
{ Float1, Float2, Float3, Float4, Float5, Float6, Float7, Float8, Float9, Float10, Float11, Float12, Float13, Float14, Float15, Float16 };
UENUM(BlueprintType)
enum class EPreReplicatedVector : uint8
{ Vector1, Vector2, Vector3, Vector4, Vector5, Vector6, Vector7, Vector8, Vector9, Vector10, Vector11, Vector12, Vector13, Vector14, Vector15, Vector16 };
UENUM(BlueprintType)
enum class EPreReplicatedNormal : uint8
{ Normal1, Normal2, Normal3, Normal4, Normal5, Normal6, Normal7, Normal8, Normal9, Normal10, Normal11, Normal12, Normal13, Normal14, Normal15, Normal16 };
UENUM(BlueprintType)
enum class EPreReplicatedRotator : uint8
{ Rotator1, Rotator2, Rotator3, Rotator4, Rotator5, Rotator6, Rotator7, Rotator8, Rotator9, Rotator10, Rotator11, Rotator12, Rotator13, Rotator14, Rotator15, Rotator16 };
UENUM(BlueprintType)
enum class EPreReplicatedActorReference : uint8
{ ActorReference1, ActorReference2, ActorReference3, ActorReference4, ActorReference5, ActorReference6, ActorReference7, ActorReference8, ActorReference9, ActorReference10, ActorReference11, ActorReference12, ActorReference13, ActorReference14, ActorReference15, ActorReference16 };
UENUM(BlueprintType)
enum class EPreReplicatedActorComponentReference : uint8
{ ActorComponentReference1, ActorComponentReference2, ActorComponentReference3, ActorComponentReference4, ActorComponentReference5, ActorComponentReference6, ActorComponentReference7, ActorComponentReference8, ActorComponentReference9, ActorComponentReference10, ActorComponentReference11, ActorComponentReference12, ActorComponentReference13, ActorComponentReference14, ActorComponentReference15, ActorComponentReference16 };
UENUM(BlueprintType)
enum class EPreReplicatedAnimMontageReference : uint8
{ AnimMontageReference1, AnimMontageReference2, AnimMontageReference3, AnimMontageReference4, AnimMontageReference5, AnimMontageReference6, AnimMontageReference7, AnimMontageReference8, AnimMontageReference9, AnimMontageReference10, AnimMontageReference11, AnimMontageReference12, AnimMontageReference13, AnimMontageReference14, AnimMontageReference15, AnimMontageReference16 };

//------------------------------------------------------------------------------------------------------------------------------------------
// Internals (DO NOT MODIFY)

// FMove variable definitions.
#define ADD_PREREPLICATED_TYPE_TO_MOVE(Type, Name)\
  Type In##Name##1{};\
  Type In##Name##2{};\
  Type In##Name##3{};\
  Type In##Name##4{};\
  Type In##Name##5{};\
  Type In##Name##6{};\
  Type In##Name##7{};\
  Type In##Name##8{};\
  Type In##Name##9{};\
  Type In##Name##10{};\
  Type In##Name##11{};\
  Type In##Name##12{};\
  Type In##Name##13{};\
  Type In##Name##14{};\
  Type In##Name##15{};\
  Type In##Name##16{};\
  Type Out##Name##1{};\
  Type Out##Name##2{};\
  Type Out##Name##3{};\
  Type Out##Name##4{};\
  Type Out##Name##5{};\
  Type Out##Name##6{};\
  Type Out##Name##7{};\
  Type Out##Name##8{};\
  Type Out##Name##9{};\
  Type Out##Name##10{};\
  Type Out##Name##11{};\
  Type Out##Name##12{};\
  Type Out##Name##13{};\
  Type Out##Name##14{};\
  Type Out##Name##15{};\
  Type Out##Name##16{};

// FState variable definitions. The bForceNetUpdate flags are only used for the simulated proxy server state, as there should not be any
// reason to force a net update to the autonomous proxy from bound data.
#define ADD_PREREPLICATED_TYPE_TO_STATE(Type, Name)\
  Type Name##1{};\
  Type Name##2{};\
  Type Name##3{};\
  Type Name##4{};\
  Type Name##5{};\
  Type Name##6{};\
  Type Name##7{};\
  Type Name##8{};\
  Type Name##9{};\
  Type Name##10{};\
  Type Name##11{};\
  Type Name##12{};\
  Type Name##13{};\
  Type Name##14{};\
  Type Name##15{};\
  Type Name##16{};\
  bool bReplicate##Name##1{false};\
  bool bReplicate##Name##2{false};\
  bool bReplicate##Name##3{false};\
  bool bReplicate##Name##4{false};\
  bool bReplicate##Name##5{false};\
  bool bReplicate##Name##6{false};\
  bool bReplicate##Name##7{false};\
  bool bReplicate##Name##8{false};\
  bool bReplicate##Name##9{false};\
  bool bReplicate##Name##10{false};\
  bool bReplicate##Name##11{false};\
  bool bReplicate##Name##12{false};\
  bool bReplicate##Name##13{false};\
  bool bReplicate##Name##14{false};\
  bool bReplicate##Name##15{false};\
  bool bReplicate##Name##16{false};\
  bool bForceNetUpdate##Name##1{false};\
  bool bForceNetUpdate##Name##2{false};\
  bool bForceNetUpdate##Name##3{false};\
  bool bForceNetUpdate##Name##4{false};\
  bool bForceNetUpdate##Name##5{false};\
  bool bForceNetUpdate##Name##6{false};\
  bool bForceNetUpdate##Name##7{false};\
  bool bForceNetUpdate##Name##8{false};\
  bool bForceNetUpdate##Name##9{false};\
  bool bForceNetUpdate##Name##10{false};\
  bool bForceNetUpdate##Name##11{false};\
  bool bForceNetUpdate##Name##12{false};\
  bool bForceNetUpdate##Name##13{false};\
  bool bForceNetUpdate##Name##14{false};\
  bool bForceNetUpdate##Name##15{false};\
  bool bForceNetUpdate##Name##16{false};\
  bool bReadNew##Name##1{false};\
  bool bReadNew##Name##2{false};\
  bool bReadNew##Name##3{false};\
  bool bReadNew##Name##4{false};\
  bool bReadNew##Name##5{false};\
  bool bReadNew##Name##6{false};\
  bool bReadNew##Name##7{false};\
  bool bReadNew##Name##8{false};\
  bool bReadNew##Name##9{false};\
  bool bReadNew##Name##10{false};\
  bool bReadNew##Name##11{false};\
  bool bReadNew##Name##12{false};\
  bool bReadNew##Name##13{false};\
  bool bReadNew##Name##14{false};\
  bool bReadNew##Name##15{false};\
  bool bReadNew##Name##16{false};

#define ADD_PREREPLICATED_TYPE_TO_REDUCED_STATE(Type, Name)\
  Type Name##1{};\
  Type Name##2{};\
  Type Name##3{};\
  Type Name##4{};\
  Type Name##5{};\
  Type Name##6{};\
  Type Name##7{};\
  Type Name##8{};\
  Type Name##9{};\
  Type Name##10{};\
  Type Name##11{};\
  Type Name##12{};\
  Type Name##13{};\
  Type Name##14{};\
  Type Name##15{};\
  Type Name##16{};

// Replication component pointer definitions.
#define ADD_PREREPLICATED_TYPE_PTR(Type, Name)\
  Type* Name##1{nullptr};\
  Type* Name##2{nullptr};\
  Type* Name##3{nullptr};\
  Type* Name##4{nullptr};\
  Type* Name##5{nullptr};\
  Type* Name##6{nullptr};\
  Type* Name##7{nullptr};\
  Type* Name##8{nullptr};\
  Type* Name##9{nullptr};\
  Type* Name##10{nullptr};\
  Type* Name##11{nullptr};\
  Type* Name##12{nullptr};\
  Type* Name##13{nullptr};\
  Type* Name##14{nullptr};\
  Type* Name##15{nullptr};\
  Type* Name##16{nullptr};

// Replication component lock definitions. These are used to lock a variable saved to the simulated proxy server state to prevent it from
// changing again before it can be replicated. Variables are only locked if they are replicated to simulated proxies and they are configured
// to force a net update upon changing, and if the min hold time is set to a value greater than 0.
#define ADD_PREREPLICATED_TYPE_LOCK(Name)\
  bool bLocked##Name##1{false};\
  bool bLocked##Name##2{false};\
  bool bLocked##Name##3{false};\
  bool bLocked##Name##4{false};\
  bool bLocked##Name##5{false};\
  bool bLocked##Name##6{false};\
  bool bLocked##Name##7{false};\
  bool bLocked##Name##8{false};\
  bool bLocked##Name##9{false};\
  bool bLocked##Name##10{false};\
  bool bLocked##Name##11{false};\
  bool bLocked##Name##12{false};\
  bool bLocked##Name##13{false};\
  bool bLocked##Name##14{false};\
  bool bLocked##Name##15{false};\
  bool bLocked##Name##16{false};\
  float ResetLockTimer##Name##1{0.f};\
  float ResetLockTimer##Name##2{0.f};\
  float ResetLockTimer##Name##3{0.f};\
  float ResetLockTimer##Name##4{0.f};\
  float ResetLockTimer##Name##5{0.f};\
  float ResetLockTimer##Name##6{0.f};\
  float ResetLockTimer##Name##7{0.f};\
  float ResetLockTimer##Name##8{0.f};\
  float ResetLockTimer##Name##9{0.f};\
  float ResetLockTimer##Name##10{0.f};\
  float ResetLockTimer##Name##11{0.f};\
  float ResetLockTimer##Name##12{0.f};\
  float ResetLockTimer##Name##13{0.f};\
  float ResetLockTimer##Name##14{0.f};\
  float ResetLockTimer##Name##15{0.f};\
  float ResetLockTimer##Name##16{0.f};\
  float LockSetTime##Name##1{0.f};\
  float LockSetTime##Name##2{0.f};\
  float LockSetTime##Name##3{0.f};\
  float LockSetTime##Name##4{0.f};\
  float LockSetTime##Name##5{0.f};\
  float LockSetTime##Name##6{0.f};\
  float LockSetTime##Name##7{0.f};\
  float LockSetTime##Name##8{0.f};\
  float LockSetTime##Name##9{0.f};\
  float LockSetTime##Name##10{0.f};\
  float LockSetTime##Name##11{0.f};\
  float LockSetTime##Name##12{0.f};\
  float LockSetTime##Name##13{0.f};\
  float LockSetTime##Name##14{0.f};\
  float LockSetTime##Name##15{0.f};\
  float LockSetTime##Name##16{0.f};

// Replication component counter definitions. Indicate how many pre-replicated variables of a particular type are currently bound.
#define ADD_PREREPLICATED_TYPE_COUNTER(Name)\
  int32 Name##Counter{0};

// Implementation calls.
#define CALL_SwapStateBufferBoundData(Name)           Server_SwapStateBuffer##Name();
#define CALL_ForceNetUpdateCheckBoundData(Name)       if (Server_ForceNetUpdateCheck##Name()) bShouldForceNetUpdate = true;
#define CALL_SaveBoundDataToServerState(Name)         Server_Save##Name##ToServerState(ServerState, RecipientRole);
#define CALL_ResetLockedBoundData(Name)               Server_ResetLocked##Name();
#define CALL_UnpackBoundData(Name)                    Client_Unpack##Name(ServerState);
#define CALL_IsBoundDataValidSpecific(Name)           if (!Client_IsValid##Name(SourceMove)) return false;
#define CALL_LoadBoundDataForReplay(Name)             Client_LoadForReplay##Name(SourceMove);
#define CALL_SaveInBoundDataToMove(Name)              Save##Name##ToMove(Move, FMove::EStateVars::Input);
#define CALL_SaveOutBoundDataToMove(Name)             Save##Name##ToMove(Move, FMove::EStateVars::Output);
#define CALL_SaveBoundDataToState(Name)               Save##Name##ToState(State);
#define CALL_LoadInBoundDataFromMove(Name)            Load##Name##FromMove(Move, FMove::EStateVars::Input);
#define CALL_LoadBoundDataFromState(Name)             Load##Name##FromState(State);
#define CALL_LoadReplicatedBoundDataFromState(Name)   LoadReplicated##Name##FromState(State);
#define CALL_AddTargetStateBoundDataToInitState(Name) AddFromTargetState##Name(InitializationState, TargetState);
#define CALL_AddStartStateBoundDataToInitState(Name)  AddFromStartState##Name(InitializationState, StartState);
#define CALL_SerializeBoundDataSpecific(Name)         Serialize##Name##Types(Ar);

// Replication logic.
#define ADD_REPLICATION_LOGIC(Type, Name)\
public:\
  void Bind##Name(Type& VariableToBind, bool bReplicateToAutonomousProxy, bool bReplicateToSimulatedProxy, bool bForceNetUpdateOnChange)\
  {\
    check(Name##Counter >= 0)\
    if (!Name##1)  { Name##1  = &VariableToBind; check(Name##Counter == 0)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##1  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##1  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##1  = true; } return; }\
    if (!Name##2)  { Name##2  = &VariableToBind; check(Name##Counter == 1)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##2  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##2  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##2  = true; } return; }\
    if (!Name##3)  { Name##3  = &VariableToBind; check(Name##Counter == 2)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##3  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##3  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##3  = true; } return; }\
    if (!Name##4)  { Name##4  = &VariableToBind; check(Name##Counter == 3)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##4  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##4  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##4  = true; } return; }\
    if (!Name##5)  { Name##5  = &VariableToBind; check(Name##Counter == 4)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##5  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##5  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##5  = true; } return; }\
    if (!Name##6)  { Name##6  = &VariableToBind; check(Name##Counter == 5)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##6  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##6  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##6  = true; } return; }\
    if (!Name##7)  { Name##7  = &VariableToBind; check(Name##Counter == 6)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##7  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##7  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##7  = true; } return; }\
    if (!Name##8)  { Name##8  = &VariableToBind; check(Name##Counter == 7)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##8  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##8  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##8  = true; } return; }\
    if (!Name##9)  { Name##9  = &VariableToBind; check(Name##Counter == 8)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##9  = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##9  = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##9  = true; } return; }\
    if (!Name##10) { Name##10 = &VariableToBind; check(Name##Counter == 9)  ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##10 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##10 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##10 = true; } return; }\
    if (!Name##11) { Name##11 = &VariableToBind; check(Name##Counter == 10) ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##11 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##11 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##11 = true; } return; }\
    if (!Name##12) { Name##12 = &VariableToBind; check(Name##Counter == 11) ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##12 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##12 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##12 = true; } return; }\
    if (!Name##13) { Name##13 = &VariableToBind; check(Name##Counter == 12) ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##13 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##13 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##13 = true; } return; }\
    if (!Name##14) { Name##14 = &VariableToBind; check(Name##Counter == 13) ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##14 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##14 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##14 = true; } return; }\
    if (!Name##15) { Name##15 = &VariableToBind; check(Name##Counter == 14) ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##15 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##15 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##15 = true; } return; }\
    if (!Name##16) { Name##16 = &VariableToBind; check(Name##Counter == 15) ++Name##Counter; if (bReplicateToAutonomousProxy) { ServerState_AutonomousProxy().bReplicate##Name##16 = true; } if (bReplicateToSimulatedProxy) { ServerState_SimulatedProxy().bReplicate##Name##16 = true; } if (bForceNetUpdateOnChange) { ServerState_SimulatedProxy().bForceNetUpdate##Name##16 = true; } return; }\
    check(Name##Counter == 16)\
    ensureAlwaysMsgf(false, TEXT("No more data available to bind for %s of type %s."), TEXT(#Name), TEXT(#Type));\
  }\
  void Bind##Name##WithAccessor(Type& VariableToBind, EPreReplicated##Name& OutAccessor, bool bReplicateToAutonomousProxy, bool bReplicateToSimulatedProxy, bool bForceNetUpdateOnChange)\
  {\
    OutAccessor = (EPreReplicated##Name)Name##Counter;\
    Bind##Name(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);\
  }\
  Type GetPreReplicated##Name##FromState(const FState& State, EPreReplicated##Name Variable) const\
  {\
    switch (Variable)\
    {\
      case EPreReplicated##Name::Name##1:  return State.Name##1;\
      case EPreReplicated##Name::Name##2:  return State.Name##2;\
      case EPreReplicated##Name::Name##3:  return State.Name##3;\
      case EPreReplicated##Name::Name##4:  return State.Name##4;\
      case EPreReplicated##Name::Name##5:  return State.Name##5;\
      case EPreReplicated##Name::Name##6:  return State.Name##6;\
      case EPreReplicated##Name::Name##7:  return State.Name##7;\
      case EPreReplicated##Name::Name##8:  return State.Name##8;\
      case EPreReplicated##Name::Name##9:  return State.Name##9;\
      case EPreReplicated##Name::Name##10: return State.Name##10;\
      case EPreReplicated##Name::Name##11: return State.Name##11;\
      case EPreReplicated##Name::Name##12: return State.Name##12;\
      case EPreReplicated##Name::Name##13: return State.Name##13;\
      case EPreReplicated##Name::Name##14: return State.Name##14;\
      case EPreReplicated##Name::Name##15: return State.Name##15;\
      case EPreReplicated##Name::Name##16: return State.Name##16;\
      default: checkNoEntry();\
    }\
    checkNoEntry()\
    return {};\
  }\
private:\
  void Server_SwapStateBuffer##Name()\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_Authority)\
    if (Name##1)  { const auto Swap##Name##1  = *Name##1;  *Name##1  = StateBuffer().Name##1;  StateBuffer().Name##1  = Swap##Name##1;  } else return;\
    if (Name##2)  { const auto Swap##Name##2  = *Name##2;  *Name##2  = StateBuffer().Name##2;  StateBuffer().Name##2  = Swap##Name##2;  } else return;\
    if (Name##3)  { const auto Swap##Name##3  = *Name##3;  *Name##3  = StateBuffer().Name##3;  StateBuffer().Name##3  = Swap##Name##3;  } else return;\
    if (Name##4)  { const auto Swap##Name##4  = *Name##4;  *Name##4  = StateBuffer().Name##4;  StateBuffer().Name##4  = Swap##Name##4;  } else return;\
    if (Name##5)  { const auto Swap##Name##5  = *Name##5;  *Name##5  = StateBuffer().Name##5;  StateBuffer().Name##5  = Swap##Name##5;  } else return;\
    if (Name##6)  { const auto Swap##Name##6  = *Name##6;  *Name##6  = StateBuffer().Name##6;  StateBuffer().Name##6  = Swap##Name##6;  } else return;\
    if (Name##7)  { const auto Swap##Name##7  = *Name##7;  *Name##7  = StateBuffer().Name##7;  StateBuffer().Name##7  = Swap##Name##7;  } else return;\
    if (Name##8)  { const auto Swap##Name##8  = *Name##8;  *Name##8  = StateBuffer().Name##8;  StateBuffer().Name##8  = Swap##Name##8;  } else return;\
    if (Name##9)  { const auto Swap##Name##9  = *Name##9;  *Name##9  = StateBuffer().Name##9;  StateBuffer().Name##9  = Swap##Name##9;  } else return;\
    if (Name##10) { const auto Swap##Name##10 = *Name##10; *Name##10 = StateBuffer().Name##10; StateBuffer().Name##10 = Swap##Name##10; } else return;\
    if (Name##11) { const auto Swap##Name##11 = *Name##11; *Name##11 = StateBuffer().Name##11; StateBuffer().Name##11 = Swap##Name##11; } else return;\
    if (Name##12) { const auto Swap##Name##12 = *Name##12; *Name##12 = StateBuffer().Name##12; StateBuffer().Name##12 = Swap##Name##12; } else return;\
    if (Name##13) { const auto Swap##Name##13 = *Name##13; *Name##13 = StateBuffer().Name##13; StateBuffer().Name##13 = Swap##Name##13; } else return;\
    if (Name##14) { const auto Swap##Name##14 = *Name##14; *Name##14 = StateBuffer().Name##14; StateBuffer().Name##14 = Swap##Name##14; } else return;\
    if (Name##15) { const auto Swap##Name##15 = *Name##15; *Name##15 = StateBuffer().Name##15; StateBuffer().Name##15 = Swap##Name##15; } else return;\
    if (Name##16) { const auto Swap##Name##16 = *Name##16; *Name##16 = StateBuffer().Name##16; StateBuffer().Name##16 = Swap##Name##16; } else return;\
  }\
  bool Server_ForceNetUpdateCheck##Name()\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_Authority)\
    const auto& ServerState = ServerState_SimulatedProxy();\
    const auto& LastServerState = ServerState_SimulatedProxy_Buffered();\
    bool bShouldForceNetUpdate = false;\
    if (Name##1)  { if (ServerState.bReplicate##Name##1  && ServerState.bForceNetUpdate##Name##1  && !bLocked##Name##1)  { if (ServerState.Name##1  != LastServerState.Name##1)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##1  = true; LockSetTime##Name##1  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##2)  { if (ServerState.bReplicate##Name##2  && ServerState.bForceNetUpdate##Name##2  && !bLocked##Name##2)  { if (ServerState.Name##2  != LastServerState.Name##2)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##2  = true; LockSetTime##Name##2  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##3)  { if (ServerState.bReplicate##Name##3  && ServerState.bForceNetUpdate##Name##3  && !bLocked##Name##3)  { if (ServerState.Name##3  != LastServerState.Name##3)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##3  = true; LockSetTime##Name##3  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##4)  { if (ServerState.bReplicate##Name##4  && ServerState.bForceNetUpdate##Name##4  && !bLocked##Name##4)  { if (ServerState.Name##4  != LastServerState.Name##4)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##4  = true; LockSetTime##Name##4  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##5)  { if (ServerState.bReplicate##Name##5  && ServerState.bForceNetUpdate##Name##5  && !bLocked##Name##5)  { if (ServerState.Name##5  != LastServerState.Name##5)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##5  = true; LockSetTime##Name##5  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##6)  { if (ServerState.bReplicate##Name##6  && ServerState.bForceNetUpdate##Name##6  && !bLocked##Name##6)  { if (ServerState.Name##6  != LastServerState.Name##6)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##6  = true; LockSetTime##Name##6  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##7)  { if (ServerState.bReplicate##Name##7  && ServerState.bForceNetUpdate##Name##7  && !bLocked##Name##7)  { if (ServerState.Name##7  != LastServerState.Name##7)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##7  = true; LockSetTime##Name##7  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##8)  { if (ServerState.bReplicate##Name##8  && ServerState.bForceNetUpdate##Name##8  && !bLocked##Name##8)  { if (ServerState.Name##8  != LastServerState.Name##8)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##8  = true; LockSetTime##Name##8  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##9)  { if (ServerState.bReplicate##Name##9  && ServerState.bForceNetUpdate##Name##9  && !bLocked##Name##9)  { if (ServerState.Name##9  != LastServerState.Name##9)  { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##9  = true; LockSetTime##Name##9  = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##10) { if (ServerState.bReplicate##Name##10 && ServerState.bForceNetUpdate##Name##10 && !bLocked##Name##10) { if (ServerState.Name##10 != LastServerState.Name##10) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##10 = true; LockSetTime##Name##10 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##11) { if (ServerState.bReplicate##Name##11 && ServerState.bForceNetUpdate##Name##11 && !bLocked##Name##11) { if (ServerState.Name##11 != LastServerState.Name##11) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##11 = true; LockSetTime##Name##11 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##12) { if (ServerState.bReplicate##Name##12 && ServerState.bForceNetUpdate##Name##12 && !bLocked##Name##12) { if (ServerState.Name##12 != LastServerState.Name##12) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##12 = true; LockSetTime##Name##12 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##13) { if (ServerState.bReplicate##Name##13 && ServerState.bForceNetUpdate##Name##13 && !bLocked##Name##13) { if (ServerState.Name##13 != LastServerState.Name##13) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##13 = true; LockSetTime##Name##13 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##14) { if (ServerState.bReplicate##Name##14 && ServerState.bForceNetUpdate##Name##14 && !bLocked##Name##14) { if (ServerState.Name##14 != LastServerState.Name##14) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##14 = true; LockSetTime##Name##14 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##15) { if (ServerState.bReplicate##Name##15 && ServerState.bForceNetUpdate##Name##15 && !bLocked##Name##15) { if (ServerState.Name##15 != LastServerState.Name##15) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##15 = true; LockSetTime##Name##15 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    if (Name##16) { if (ServerState.bReplicate##Name##16 && ServerState.bForceNetUpdate##Name##16 && !bLocked##Name##16) { if (ServerState.Name##16 != LastServerState.Name##16) { bShouldForceNetUpdate = true; if (MinRepHoldTime > 0.f) { bLocked##Name##16 = true; LockSetTime##Name##16 = GetTime(); } } } } else return bShouldForceNetUpdate;\
    return bShouldForceNetUpdate;\
  }\
  void Server_Save##Name##ToServerState(FState& ServerState, ENetRole RecipientRole) const\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_Authority)\
    check(RecipientRole == ROLE_AutonomousProxy || RecipientRole == ROLE_SimulatedProxy)\
    if (RecipientRole == ROLE_AutonomousProxy)\
    {\
      if (Name##1)  ServerState.Name##1  = *Name##1;  else return;\
      if (Name##2)  ServerState.Name##2  = *Name##2;  else return;\
      if (Name##3)  ServerState.Name##3  = *Name##3;  else return;\
      if (Name##4)  ServerState.Name##4  = *Name##4;  else return;\
      if (Name##5)  ServerState.Name##5  = *Name##5;  else return;\
      if (Name##6)  ServerState.Name##6  = *Name##6;  else return;\
      if (Name##7)  ServerState.Name##7  = *Name##7;  else return;\
      if (Name##8)  ServerState.Name##8  = *Name##8;  else return;\
      if (Name##9)  ServerState.Name##9  = *Name##9;  else return;\
      if (Name##10) ServerState.Name##10 = *Name##10; else return;\
      if (Name##11) ServerState.Name##11 = *Name##11; else return;\
      if (Name##12) ServerState.Name##12 = *Name##12; else return;\
      if (Name##13) ServerState.Name##13 = *Name##13; else return;\
      if (Name##14) ServerState.Name##14 = *Name##14; else return;\
      if (Name##15) ServerState.Name##15 = *Name##15; else return;\
      if (Name##16) ServerState.Name##16 = *Name##16; else return;\
      return;\
    }\
    if (RecipientRole == ROLE_SimulatedProxy)\
    {\
      if (Name##1)  { if (!bLocked##Name##1)  ServerState.Name##1  = *Name##1;  } else return;\
      if (Name##2)  { if (!bLocked##Name##2)  ServerState.Name##2  = *Name##2;  } else return;\
      if (Name##3)  { if (!bLocked##Name##3)  ServerState.Name##3  = *Name##3;  } else return;\
      if (Name##4)  { if (!bLocked##Name##4)  ServerState.Name##4  = *Name##4;  } else return;\
      if (Name##5)  { if (!bLocked##Name##5)  ServerState.Name##5  = *Name##5;  } else return;\
      if (Name##6)  { if (!bLocked##Name##6)  ServerState.Name##6  = *Name##6;  } else return;\
      if (Name##7)  { if (!bLocked##Name##7)  ServerState.Name##7  = *Name##7;  } else return;\
      if (Name##8)  { if (!bLocked##Name##8)  ServerState.Name##8  = *Name##8;  } else return;\
      if (Name##9)  { if (!bLocked##Name##9)  ServerState.Name##9  = *Name##9;  } else return;\
      if (Name##10) { if (!bLocked##Name##10) ServerState.Name##10 = *Name##10; } else return;\
      if (Name##11) { if (!bLocked##Name##11) ServerState.Name##11 = *Name##11; } else return;\
      if (Name##12) { if (!bLocked##Name##12) ServerState.Name##12 = *Name##12; } else return;\
      if (Name##13) { if (!bLocked##Name##13) ServerState.Name##13 = *Name##13; } else return;\
      if (Name##14) { if (!bLocked##Name##14) ServerState.Name##14 = *Name##14; } else return;\
      if (Name##15) { if (!bLocked##Name##15) ServerState.Name##15 = *Name##15; } else return;\
      if (Name##16) { if (!bLocked##Name##16) ServerState.Name##16 = *Name##16; } else return;\
      return;\
    }\
    checkNoEntry()\
  }\
  void Server_ResetLocked##Name()\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_Authority)\
    const float CurrentTime = GetTime();\
    if (bLocked##Name##1)  { ResetLockTimer##Name##1  = CurrentTime - LockSetTime##Name##1;  if (ResetLockTimer##Name##1  > MinRepHoldTime) { bLocked##Name##1  = false; ResetLockTimer##Name##1  = 0.f; } }\
    if (bLocked##Name##2)  { ResetLockTimer##Name##2  = CurrentTime - LockSetTime##Name##2;  if (ResetLockTimer##Name##2  > MinRepHoldTime) { bLocked##Name##2  = false; ResetLockTimer##Name##2  = 0.f; } }\
    if (bLocked##Name##3)  { ResetLockTimer##Name##3  = CurrentTime - LockSetTime##Name##3;  if (ResetLockTimer##Name##3  > MinRepHoldTime) { bLocked##Name##3  = false; ResetLockTimer##Name##3  = 0.f; } }\
    if (bLocked##Name##4)  { ResetLockTimer##Name##4  = CurrentTime - LockSetTime##Name##4;  if (ResetLockTimer##Name##4  > MinRepHoldTime) { bLocked##Name##4  = false; ResetLockTimer##Name##4  = 0.f; } }\
    if (bLocked##Name##5)  { ResetLockTimer##Name##5  = CurrentTime - LockSetTime##Name##5;  if (ResetLockTimer##Name##5  > MinRepHoldTime) { bLocked##Name##5  = false; ResetLockTimer##Name##5  = 0.f; } }\
    if (bLocked##Name##6)  { ResetLockTimer##Name##6  = CurrentTime - LockSetTime##Name##6;  if (ResetLockTimer##Name##6  > MinRepHoldTime) { bLocked##Name##6  = false; ResetLockTimer##Name##6  = 0.f; } }\
    if (bLocked##Name##7)  { ResetLockTimer##Name##7  = CurrentTime - LockSetTime##Name##7;  if (ResetLockTimer##Name##7  > MinRepHoldTime) { bLocked##Name##7  = false; ResetLockTimer##Name##7  = 0.f; } }\
    if (bLocked##Name##8)  { ResetLockTimer##Name##8  = CurrentTime - LockSetTime##Name##8;  if (ResetLockTimer##Name##8  > MinRepHoldTime) { bLocked##Name##8  = false; ResetLockTimer##Name##8  = 0.f; } }\
    if (bLocked##Name##9)  { ResetLockTimer##Name##9  = CurrentTime - LockSetTime##Name##9;  if (ResetLockTimer##Name##9  > MinRepHoldTime) { bLocked##Name##9  = false; ResetLockTimer##Name##9  = 0.f; } }\
    if (bLocked##Name##10) { ResetLockTimer##Name##10 = CurrentTime - LockSetTime##Name##10; if (ResetLockTimer##Name##10 > MinRepHoldTime) { bLocked##Name##10 = false; ResetLockTimer##Name##10 = 0.f; } }\
    if (bLocked##Name##11) { ResetLockTimer##Name##11 = CurrentTime - LockSetTime##Name##11; if (ResetLockTimer##Name##11 > MinRepHoldTime) { bLocked##Name##11 = false; ResetLockTimer##Name##11 = 0.f; } }\
    if (bLocked##Name##12) { ResetLockTimer##Name##12 = CurrentTime - LockSetTime##Name##12; if (ResetLockTimer##Name##12 > MinRepHoldTime) { bLocked##Name##12 = false; ResetLockTimer##Name##12 = 0.f; } }\
    if (bLocked##Name##13) { ResetLockTimer##Name##13 = CurrentTime - LockSetTime##Name##13; if (ResetLockTimer##Name##13 > MinRepHoldTime) { bLocked##Name##13 = false; ResetLockTimer##Name##13 = 0.f; } }\
    if (bLocked##Name##14) { ResetLockTimer##Name##14 = CurrentTime - LockSetTime##Name##14; if (ResetLockTimer##Name##14 > MinRepHoldTime) { bLocked##Name##14 = false; ResetLockTimer##Name##14 = 0.f; } }\
    if (bLocked##Name##15) { ResetLockTimer##Name##15 = CurrentTime - LockSetTime##Name##15; if (ResetLockTimer##Name##15 > MinRepHoldTime) { bLocked##Name##15 = false; ResetLockTimer##Name##15 = 0.f; } }\
    if (bLocked##Name##16) { ResetLockTimer##Name##16 = CurrentTime - LockSetTime##Name##16; if (ResetLockTimer##Name##16 > MinRepHoldTime) { bLocked##Name##16 = false; ResetLockTimer##Name##16 = 0.f; } }\
  }\
  void Client_Unpack##Name(FState& ServerState) const\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_AutonomousProxy || PawnOwner->GetLocalRole() == ROLE_SimulatedProxy)\
    check(ServerState.RecipientRole == ROLE_AutonomousProxy || ServerState.RecipientRole == ROLE_SimulatedProxy)\
    if (ServerState.bSerializeBoundData)\
    {\
      auto& SS = ServerState;\
      const auto& LRSS = Client_LastReceivedServerState;\
      if (Name##1)  { if (SS.bReplicate##Name##1)  if (!SS.bReadNew##Name##1)  SS.Name##1  = LRSS.Name##1;  } else return;\
      if (Name##2)  { if (SS.bReplicate##Name##2)  if (!SS.bReadNew##Name##2)  SS.Name##2  = LRSS.Name##2;  } else return;\
      if (Name##3)  { if (SS.bReplicate##Name##3)  if (!SS.bReadNew##Name##3)  SS.Name##3  = LRSS.Name##3;  } else return;\
      if (Name##4)  { if (SS.bReplicate##Name##4)  if (!SS.bReadNew##Name##4)  SS.Name##4  = LRSS.Name##4;  } else return;\
      if (Name##5)  { if (SS.bReplicate##Name##5)  if (!SS.bReadNew##Name##5)  SS.Name##5  = LRSS.Name##5;  } else return;\
      if (Name##6)  { if (SS.bReplicate##Name##6)  if (!SS.bReadNew##Name##6)  SS.Name##6  = LRSS.Name##6;  } else return;\
      if (Name##7)  { if (SS.bReplicate##Name##7)  if (!SS.bReadNew##Name##7)  SS.Name##7  = LRSS.Name##7;  } else return;\
      if (Name##8)  { if (SS.bReplicate##Name##8)  if (!SS.bReadNew##Name##8)  SS.Name##8  = LRSS.Name##8;  } else return;\
      if (Name##9)  { if (SS.bReplicate##Name##9)  if (!SS.bReadNew##Name##9)  SS.Name##9  = LRSS.Name##9;  } else return;\
      if (Name##10) { if (SS.bReplicate##Name##10) if (!SS.bReadNew##Name##10) SS.Name##10 = LRSS.Name##10; } else return;\
      if (Name##11) { if (SS.bReplicate##Name##11) if (!SS.bReadNew##Name##11) SS.Name##11 = LRSS.Name##11; } else return;\
      if (Name##12) { if (SS.bReplicate##Name##12) if (!SS.bReadNew##Name##12) SS.Name##12 = LRSS.Name##12; } else return;\
      if (Name##13) { if (SS.bReplicate##Name##13) if (!SS.bReadNew##Name##13) SS.Name##13 = LRSS.Name##13; } else return;\
      if (Name##14) { if (SS.bReplicate##Name##14) if (!SS.bReadNew##Name##14) SS.Name##14 = LRSS.Name##14; } else return;\
      if (Name##15) { if (SS.bReplicate##Name##15) if (!SS.bReadNew##Name##15) SS.Name##15 = LRSS.Name##15; } else return;\
      if (Name##16) { if (SS.bReplicate##Name##16) if (!SS.bReadNew##Name##16) SS.Name##16 = LRSS.Name##16; } else return;\
    }\
  }\
  void Client_LoadForReplay##Name(const FMove& SourceMove)\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_AutonomousProxy)\
    const auto& ServerState = ServerState_AutonomousProxy();\
    if (Name##1)  { if (ServerState.bReplicate##Name##1)  *Name##1  = ServerState.Name##1;  else *Name##1  = SourceMove.Out##Name##1;  } else return;\
    if (Name##2)  { if (ServerState.bReplicate##Name##2)  *Name##2  = ServerState.Name##2;  else *Name##2  = SourceMove.Out##Name##2;  } else return;\
    if (Name##3)  { if (ServerState.bReplicate##Name##3)  *Name##3  = ServerState.Name##3;  else *Name##3  = SourceMove.Out##Name##3;  } else return;\
    if (Name##4)  { if (ServerState.bReplicate##Name##4)  *Name##4  = ServerState.Name##4;  else *Name##4  = SourceMove.Out##Name##4;  } else return;\
    if (Name##5)  { if (ServerState.bReplicate##Name##5)  *Name##5  = ServerState.Name##5;  else *Name##5  = SourceMove.Out##Name##5;  } else return;\
    if (Name##6)  { if (ServerState.bReplicate##Name##6)  *Name##6  = ServerState.Name##6;  else *Name##6  = SourceMove.Out##Name##6;  } else return;\
    if (Name##7)  { if (ServerState.bReplicate##Name##7)  *Name##7  = ServerState.Name##7;  else *Name##7  = SourceMove.Out##Name##7;  } else return;\
    if (Name##8)  { if (ServerState.bReplicate##Name##8)  *Name##8  = ServerState.Name##8;  else *Name##8  = SourceMove.Out##Name##8;  } else return;\
    if (Name##9)  { if (ServerState.bReplicate##Name##9)  *Name##9  = ServerState.Name##9;  else *Name##9  = SourceMove.Out##Name##9;  } else return;\
    if (Name##10) { if (ServerState.bReplicate##Name##10) *Name##10 = ServerState.Name##10; else *Name##10 = SourceMove.Out##Name##10; } else return;\
    if (Name##11) { if (ServerState.bReplicate##Name##11) *Name##11 = ServerState.Name##11; else *Name##11 = SourceMove.Out##Name##11; } else return;\
    if (Name##12) { if (ServerState.bReplicate##Name##12) *Name##12 = ServerState.Name##12; else *Name##12 = SourceMove.Out##Name##12; } else return;\
    if (Name##13) { if (ServerState.bReplicate##Name##13) *Name##13 = ServerState.Name##13; else *Name##13 = SourceMove.Out##Name##13; } else return;\
    if (Name##14) { if (ServerState.bReplicate##Name##14) *Name##14 = ServerState.Name##14; else *Name##14 = SourceMove.Out##Name##14; } else return;\
    if (Name##15) { if (ServerState.bReplicate##Name##15) *Name##15 = ServerState.Name##15; else *Name##15 = SourceMove.Out##Name##15; } else return;\
    if (Name##16) { if (ServerState.bReplicate##Name##16) *Name##16 = ServerState.Name##16; else *Name##16 = SourceMove.Out##Name##16; } else return;\
  }\
  void Save##Name##ToMove(FMove& Move, FMove::EStateVars VarsToSave) const\
  {\
    if (VarsToSave == FMove::EStateVars::Input)\
    {\
      if (Name##1)  Move.In##Name##1  = *Name##1;  else return;\
      if (Name##2)  Move.In##Name##2  = *Name##2;  else return;\
      if (Name##3)  Move.In##Name##3  = *Name##3;  else return;\
      if (Name##4)  Move.In##Name##4  = *Name##4;  else return;\
      if (Name##5)  Move.In##Name##5  = *Name##5;  else return;\
      if (Name##6)  Move.In##Name##6  = *Name##6;  else return;\
      if (Name##7)  Move.In##Name##7  = *Name##7;  else return;\
      if (Name##8)  Move.In##Name##8  = *Name##8;  else return;\
      if (Name##9)  Move.In##Name##9  = *Name##9;  else return;\
      if (Name##10) Move.In##Name##10 = *Name##10; else return;\
      if (Name##11) Move.In##Name##11 = *Name##11; else return;\
      if (Name##12) Move.In##Name##12 = *Name##12; else return;\
      if (Name##13) Move.In##Name##13 = *Name##13; else return;\
      if (Name##14) Move.In##Name##14 = *Name##14; else return;\
      if (Name##15) Move.In##Name##15 = *Name##15; else return;\
      if (Name##16) Move.In##Name##16 = *Name##16; else return;\
    }\
    else if (VarsToSave == FMove::EStateVars::Output)\
    {\
      if (Name##1)  Move.Out##Name##1  = *Name##1;  else return;\
      if (Name##2)  Move.Out##Name##2  = *Name##2;  else return;\
      if (Name##3)  Move.Out##Name##3  = *Name##3;  else return;\
      if (Name##4)  Move.Out##Name##4  = *Name##4;  else return;\
      if (Name##5)  Move.Out##Name##5  = *Name##5;  else return;\
      if (Name##6)  Move.Out##Name##6  = *Name##6;  else return;\
      if (Name##7)  Move.Out##Name##7  = *Name##7;  else return;\
      if (Name##8)  Move.Out##Name##8  = *Name##8;  else return;\
      if (Name##9)  Move.Out##Name##9  = *Name##9;  else return;\
      if (Name##10) Move.Out##Name##10 = *Name##10; else return;\
      if (Name##11) Move.Out##Name##11 = *Name##11; else return;\
      if (Name##12) Move.Out##Name##12 = *Name##12; else return;\
      if (Name##13) Move.Out##Name##13 = *Name##13; else return;\
      if (Name##14) Move.Out##Name##14 = *Name##14; else return;\
      if (Name##15) Move.Out##Name##15 = *Name##15; else return;\
      if (Name##16) Move.Out##Name##16 = *Name##16; else return;\
    }\
  }\
  void Save##Name##ToState(FState& State) const\
  {\
    if (Name##1)  State.Name##1  = *Name##1;  else return;\
    if (Name##2)  State.Name##2  = *Name##2;  else return;\
    if (Name##3)  State.Name##3  = *Name##3;  else return;\
    if (Name##4)  State.Name##4  = *Name##4;  else return;\
    if (Name##5)  State.Name##5  = *Name##5;  else return;\
    if (Name##6)  State.Name##6  = *Name##6;  else return;\
    if (Name##7)  State.Name##7  = *Name##7;  else return;\
    if (Name##8)  State.Name##8  = *Name##8;  else return;\
    if (Name##9)  State.Name##9  = *Name##9;  else return;\
    if (Name##10) State.Name##10 = *Name##10; else return;\
    if (Name##11) State.Name##11 = *Name##11; else return;\
    if (Name##12) State.Name##12 = *Name##12; else return;\
    if (Name##13) State.Name##13 = *Name##13; else return;\
    if (Name##14) State.Name##14 = *Name##14; else return;\
    if (Name##15) State.Name##15 = *Name##15; else return;\
    if (Name##16) State.Name##16 = *Name##16; else return;\
  }\
  void Load##Name##FromMove(const FMove& Move, FMove::EStateVars VarsToLoad) const\
  {\
    if (VarsToLoad == FMove::EStateVars::Input)\
    {\
      if (Name##1)  *Name##1  = Move.In##Name##1;  else return;\
      if (Name##2)  *Name##2  = Move.In##Name##2;  else return;\
      if (Name##3)  *Name##3  = Move.In##Name##3;  else return;\
      if (Name##4)  *Name##4  = Move.In##Name##4;  else return;\
      if (Name##5)  *Name##5  = Move.In##Name##5;  else return;\
      if (Name##6)  *Name##6  = Move.In##Name##6;  else return;\
      if (Name##7)  *Name##7  = Move.In##Name##7;  else return;\
      if (Name##8)  *Name##8  = Move.In##Name##8;  else return;\
      if (Name##9)  *Name##9  = Move.In##Name##9;  else return;\
      if (Name##10) *Name##10 = Move.In##Name##10; else return;\
      if (Name##11) *Name##11 = Move.In##Name##11; else return;\
      if (Name##12) *Name##12 = Move.In##Name##12; else return;\
      if (Name##13) *Name##13 = Move.In##Name##13; else return;\
      if (Name##14) *Name##14 = Move.In##Name##14; else return;\
      if (Name##15) *Name##15 = Move.In##Name##15; else return;\
      if (Name##16) *Name##16 = Move.In##Name##16; else return;\
    }\
    else if (VarsToLoad == FMove::EStateVars::Output)\
    {\
      if (Name##1)  *Name##1  = Move.Out##Name##1;  else return;\
      if (Name##2)  *Name##2  = Move.Out##Name##2;  else return;\
      if (Name##3)  *Name##3  = Move.Out##Name##3;  else return;\
      if (Name##4)  *Name##4  = Move.Out##Name##4;  else return;\
      if (Name##5)  *Name##5  = Move.Out##Name##5;  else return;\
      if (Name##6)  *Name##6  = Move.Out##Name##6;  else return;\
      if (Name##7)  *Name##7  = Move.Out##Name##7;  else return;\
      if (Name##8)  *Name##8  = Move.Out##Name##8;  else return;\
      if (Name##9)  *Name##9  = Move.Out##Name##9;  else return;\
      if (Name##10) *Name##10 = Move.Out##Name##10; else return;\
      if (Name##11) *Name##11 = Move.Out##Name##11; else return;\
      if (Name##12) *Name##12 = Move.Out##Name##12; else return;\
      if (Name##13) *Name##13 = Move.Out##Name##13; else return;\
      if (Name##14) *Name##14 = Move.Out##Name##14; else return;\
      if (Name##15) *Name##15 = Move.Out##Name##15; else return;\
      if (Name##16) *Name##16 = Move.Out##Name##16; else return;\
    }\
  }\
  void Load##Name##FromState(const FState& State) const\
  {\
    if (Name##1)  { *Name##1  = State.Name##1;  } else return;\
    if (Name##2)  { *Name##2  = State.Name##2;  } else return;\
    if (Name##3)  { *Name##3  = State.Name##3;  } else return;\
    if (Name##4)  { *Name##4  = State.Name##4;  } else return;\
    if (Name##5)  { *Name##5  = State.Name##5;  } else return;\
    if (Name##6)  { *Name##6  = State.Name##6;  } else return;\
    if (Name##7)  { *Name##7  = State.Name##7;  } else return;\
    if (Name##8)  { *Name##8  = State.Name##8;  } else return;\
    if (Name##9)  { *Name##9  = State.Name##9;  } else return;\
    if (Name##10) { *Name##10 = State.Name##10; } else return;\
    if (Name##11) { *Name##11 = State.Name##11; } else return;\
    if (Name##12) { *Name##12 = State.Name##12; } else return;\
    if (Name##13) { *Name##13 = State.Name##13; } else return;\
    if (Name##14) { *Name##14 = State.Name##14; } else return;\
    if (Name##15) { *Name##15 = State.Name##15; } else return;\
    if (Name##16) { *Name##16 = State.Name##16; } else return;\
  }\
  void LoadReplicated##Name##FromState(const FState& State) const\
  {\
    if (Name##1)  { if (ServerState_SimulatedProxy().bReplicate##Name##1)  *Name##1  = State.Name##1;  } else return;\
    if (Name##2)  { if (ServerState_SimulatedProxy().bReplicate##Name##2)  *Name##2  = State.Name##2;  } else return;\
    if (Name##3)  { if (ServerState_SimulatedProxy().bReplicate##Name##3)  *Name##3  = State.Name##3;  } else return;\
    if (Name##4)  { if (ServerState_SimulatedProxy().bReplicate##Name##4)  *Name##4  = State.Name##4;  } else return;\
    if (Name##5)  { if (ServerState_SimulatedProxy().bReplicate##Name##5)  *Name##5  = State.Name##5;  } else return;\
    if (Name##6)  { if (ServerState_SimulatedProxy().bReplicate##Name##6)  *Name##6  = State.Name##6;  } else return;\
    if (Name##7)  { if (ServerState_SimulatedProxy().bReplicate##Name##7)  *Name##7  = State.Name##7;  } else return;\
    if (Name##8)  { if (ServerState_SimulatedProxy().bReplicate##Name##8)  *Name##8  = State.Name##8;  } else return;\
    if (Name##9)  { if (ServerState_SimulatedProxy().bReplicate##Name##9)  *Name##9  = State.Name##9;  } else return;\
    if (Name##10) { if (ServerState_SimulatedProxy().bReplicate##Name##10) *Name##10 = State.Name##10; } else return;\
    if (Name##11) { if (ServerState_SimulatedProxy().bReplicate##Name##11) *Name##11 = State.Name##11; } else return;\
    if (Name##12) { if (ServerState_SimulatedProxy().bReplicate##Name##12) *Name##12 = State.Name##12; } else return;\
    if (Name##13) { if (ServerState_SimulatedProxy().bReplicate##Name##13) *Name##13 = State.Name##13; } else return;\
    if (Name##14) { if (ServerState_SimulatedProxy().bReplicate##Name##14) *Name##14 = State.Name##14; } else return;\
    if (Name##15) { if (ServerState_SimulatedProxy().bReplicate##Name##15) *Name##15 = State.Name##15; } else return;\
    if (Name##16) { if (ServerState_SimulatedProxy().bReplicate##Name##16) *Name##16 = State.Name##16; } else return;\
  }\
  void AddFromTargetState##Name(FState& InitializationState, const FState& TargetState) const\
  {\
    if (IsValidStateQueueIndex(CurrentTargetStateIndex + 1)) {\
      if (TargetState.bReplicate##Name##1  && TargetState.bForceNetUpdate##Name##1)  { if (InitializationState.Name##1  != TargetState.Name##1  && InitializationState.Name##1  == StateQueue[CurrentTargetStateIndex + 1].Name##1)  InitializationState.Name##1  = TargetState.Name##1;  }\
      if (TargetState.bReplicate##Name##2  && TargetState.bForceNetUpdate##Name##2)  { if (InitializationState.Name##2  != TargetState.Name##2  && InitializationState.Name##2  == StateQueue[CurrentTargetStateIndex + 1].Name##2)  InitializationState.Name##2  = TargetState.Name##2;  }\
      if (TargetState.bReplicate##Name##3  && TargetState.bForceNetUpdate##Name##3)  { if (InitializationState.Name##3  != TargetState.Name##3  && InitializationState.Name##3  == StateQueue[CurrentTargetStateIndex + 1].Name##3)  InitializationState.Name##3  = TargetState.Name##3;  }\
      if (TargetState.bReplicate##Name##4  && TargetState.bForceNetUpdate##Name##4)  { if (InitializationState.Name##4  != TargetState.Name##4  && InitializationState.Name##4  == StateQueue[CurrentTargetStateIndex + 1].Name##4)  InitializationState.Name##4  = TargetState.Name##4;  }\
      if (TargetState.bReplicate##Name##5  && TargetState.bForceNetUpdate##Name##5)  { if (InitializationState.Name##5  != TargetState.Name##5  && InitializationState.Name##5  == StateQueue[CurrentTargetStateIndex + 1].Name##5)  InitializationState.Name##5  = TargetState.Name##5;  }\
      if (TargetState.bReplicate##Name##6  && TargetState.bForceNetUpdate##Name##6)  { if (InitializationState.Name##6  != TargetState.Name##6  && InitializationState.Name##6  == StateQueue[CurrentTargetStateIndex + 1].Name##6)  InitializationState.Name##6  = TargetState.Name##6;  }\
      if (TargetState.bReplicate##Name##7  && TargetState.bForceNetUpdate##Name##7)  { if (InitializationState.Name##7  != TargetState.Name##7  && InitializationState.Name##7  == StateQueue[CurrentTargetStateIndex + 1].Name##7)  InitializationState.Name##7  = TargetState.Name##7;  }\
      if (TargetState.bReplicate##Name##8  && TargetState.bForceNetUpdate##Name##8)  { if (InitializationState.Name##8  != TargetState.Name##8  && InitializationState.Name##8  == StateQueue[CurrentTargetStateIndex + 1].Name##8)  InitializationState.Name##8  = TargetState.Name##8;  }\
      if (TargetState.bReplicate##Name##9  && TargetState.bForceNetUpdate##Name##9)  { if (InitializationState.Name##9  != TargetState.Name##9  && InitializationState.Name##9  == StateQueue[CurrentTargetStateIndex + 1].Name##9)  InitializationState.Name##9  = TargetState.Name##9;  }\
      if (TargetState.bReplicate##Name##10 && TargetState.bForceNetUpdate##Name##10) { if (InitializationState.Name##10 != TargetState.Name##10 && InitializationState.Name##10 == StateQueue[CurrentTargetStateIndex + 1].Name##10) InitializationState.Name##10 = TargetState.Name##10; }\
      if (TargetState.bReplicate##Name##11 && TargetState.bForceNetUpdate##Name##11) { if (InitializationState.Name##11 != TargetState.Name##11 && InitializationState.Name##11 == StateQueue[CurrentTargetStateIndex + 1].Name##11) InitializationState.Name##11 = TargetState.Name##11; }\
      if (TargetState.bReplicate##Name##12 && TargetState.bForceNetUpdate##Name##12) { if (InitializationState.Name##12 != TargetState.Name##12 && InitializationState.Name##12 == StateQueue[CurrentTargetStateIndex + 1].Name##12) InitializationState.Name##12 = TargetState.Name##12; }\
      if (TargetState.bReplicate##Name##13 && TargetState.bForceNetUpdate##Name##13) { if (InitializationState.Name##13 != TargetState.Name##13 && InitializationState.Name##13 == StateQueue[CurrentTargetStateIndex + 1].Name##13) InitializationState.Name##13 = TargetState.Name##13; }\
      if (TargetState.bReplicate##Name##14 && TargetState.bForceNetUpdate##Name##14) { if (InitializationState.Name##14 != TargetState.Name##14 && InitializationState.Name##14 == StateQueue[CurrentTargetStateIndex + 1].Name##14) InitializationState.Name##14 = TargetState.Name##14; }\
      if (TargetState.bReplicate##Name##15 && TargetState.bForceNetUpdate##Name##15) { if (InitializationState.Name##15 != TargetState.Name##15 && InitializationState.Name##15 == StateQueue[CurrentTargetStateIndex + 1].Name##15) InitializationState.Name##15 = TargetState.Name##15; }\
      if (TargetState.bReplicate##Name##16 && TargetState.bForceNetUpdate##Name##16) { if (InitializationState.Name##16 != TargetState.Name##16 && InitializationState.Name##16 == StateQueue[CurrentTargetStateIndex + 1].Name##16) InitializationState.Name##16 = TargetState.Name##16; }\
    }\
  }\
  void AddFromStartState##Name(FState& InitializationState, const FState& StartState) const\
  {\
    if (IsValidStateQueueIndex(CurrentStartStateIndex - 1)) {\
      if (StartState.bReplicate##Name##1  && StartState.bForceNetUpdate##Name##1)  { if (InitializationState.Name##1  != StartState.Name##1  && InitializationState.Name##1  == StateQueue[CurrentStartStateIndex - 1].Name##1)  InitializationState.Name##1  = StartState.Name##1;  }\
      if (StartState.bReplicate##Name##2  && StartState.bForceNetUpdate##Name##2)  { if (InitializationState.Name##2  != StartState.Name##2  && InitializationState.Name##2  == StateQueue[CurrentStartStateIndex - 1].Name##2)  InitializationState.Name##2  = StartState.Name##2;  }\
      if (StartState.bReplicate##Name##3  && StartState.bForceNetUpdate##Name##3)  { if (InitializationState.Name##3  != StartState.Name##3  && InitializationState.Name##3  == StateQueue[CurrentStartStateIndex - 1].Name##3)  InitializationState.Name##3  = StartState.Name##3;  }\
      if (StartState.bReplicate##Name##4  && StartState.bForceNetUpdate##Name##4)  { if (InitializationState.Name##4  != StartState.Name##4  && InitializationState.Name##4  == StateQueue[CurrentStartStateIndex - 1].Name##4)  InitializationState.Name##4  = StartState.Name##4;  }\
      if (StartState.bReplicate##Name##5  && StartState.bForceNetUpdate##Name##5)  { if (InitializationState.Name##5  != StartState.Name##5  && InitializationState.Name##5  == StateQueue[CurrentStartStateIndex - 1].Name##5)  InitializationState.Name##5  = StartState.Name##5;  }\
      if (StartState.bReplicate##Name##6  && StartState.bForceNetUpdate##Name##6)  { if (InitializationState.Name##6  != StartState.Name##6  && InitializationState.Name##6  == StateQueue[CurrentStartStateIndex - 1].Name##6)  InitializationState.Name##6  = StartState.Name##6;  }\
      if (StartState.bReplicate##Name##7  && StartState.bForceNetUpdate##Name##7)  { if (InitializationState.Name##7  != StartState.Name##7  && InitializationState.Name##7  == StateQueue[CurrentStartStateIndex - 1].Name##7)  InitializationState.Name##7  = StartState.Name##7;  }\
      if (StartState.bReplicate##Name##8  && StartState.bForceNetUpdate##Name##8)  { if (InitializationState.Name##8  != StartState.Name##8  && InitializationState.Name##8  == StateQueue[CurrentStartStateIndex - 1].Name##8)  InitializationState.Name##8  = StartState.Name##8;  }\
      if (StartState.bReplicate##Name##9  && StartState.bForceNetUpdate##Name##9)  { if (InitializationState.Name##9  != StartState.Name##9  && InitializationState.Name##9  == StateQueue[CurrentStartStateIndex - 1].Name##9)  InitializationState.Name##9  = StartState.Name##9;  }\
      if (StartState.bReplicate##Name##10 && StartState.bForceNetUpdate##Name##10) { if (InitializationState.Name##10 != StartState.Name##10 && InitializationState.Name##10 == StateQueue[CurrentStartStateIndex - 1].Name##10) InitializationState.Name##10 = StartState.Name##10; }\
      if (StartState.bReplicate##Name##11 && StartState.bForceNetUpdate##Name##11) { if (InitializationState.Name##11 != StartState.Name##11 && InitializationState.Name##11 == StateQueue[CurrentStartStateIndex - 1].Name##11) InitializationState.Name##11 = StartState.Name##11; }\
      if (StartState.bReplicate##Name##12 && StartState.bForceNetUpdate##Name##12) { if (InitializationState.Name##12 != StartState.Name##12 && InitializationState.Name##12 == StateQueue[CurrentStartStateIndex - 1].Name##12) InitializationState.Name##12 = StartState.Name##12; }\
      if (StartState.bReplicate##Name##13 && StartState.bForceNetUpdate##Name##13) { if (InitializationState.Name##13 != StartState.Name##13 && InitializationState.Name##13 == StateQueue[CurrentStartStateIndex - 1].Name##13) InitializationState.Name##13 = StartState.Name##13; }\
      if (StartState.bReplicate##Name##14 && StartState.bForceNetUpdate##Name##14) { if (InitializationState.Name##14 != StartState.Name##14 && InitializationState.Name##14 == StateQueue[CurrentStartStateIndex - 1].Name##14) InitializationState.Name##14 = StartState.Name##14; }\
      if (StartState.bReplicate##Name##15 && StartState.bForceNetUpdate##Name##15) { if (InitializationState.Name##15 != StartState.Name##15 && InitializationState.Name##15 == StateQueue[CurrentStartStateIndex - 1].Name##15) InitializationState.Name##15 = StartState.Name##15; }\
      if (StartState.bReplicate##Name##16 && StartState.bForceNetUpdate##Name##16) { if (InitializationState.Name##16 != StartState.Name##16 && InitializationState.Name##16 == StateQueue[CurrentStartStateIndex - 1].Name##16) InitializationState.Name##16 = StartState.Name##16; }\
    }\
  }\
  ADD_PREREPLICATED_TYPE_PTR(Type, Name)\
  ADD_PREREPLICATED_TYPE_LOCK(Name)\
  ADD_PREREPLICATED_TYPE_COUNTER(Name)

// Generic bound data validation. For floating point numbers a specific validation function should be impelemented.
#define CALL_IsBoundDataValidGeneric(Name)\
  do\
  {\
    check(PawnOwner->GetLocalRole() == ROLE_AutonomousProxy)\
    const auto& ServerState = ServerState_AutonomousProxy();\
    if (Name##1)  { if (ServerState.bReplicate##Name##1  && ServerState.Name##1  != SourceMove.Out##Name##1)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "1  != SourceMove.Out" #Name "1"))  return false; } } else break;\
    if (Name##2)  { if (ServerState.bReplicate##Name##2  && ServerState.Name##2  != SourceMove.Out##Name##2)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "2  != SourceMove.Out" #Name "2"))  return false; } } else break;\
    if (Name##3)  { if (ServerState.bReplicate##Name##3  && ServerState.Name##3  != SourceMove.Out##Name##3)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "3  != SourceMove.Out" #Name "3"))  return false; } } else break;\
    if (Name##4)  { if (ServerState.bReplicate##Name##4  && ServerState.Name##4  != SourceMove.Out##Name##4)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "4  != SourceMove.Out" #Name "4"))  return false; } } else break;\
    if (Name##5)  { if (ServerState.bReplicate##Name##5  && ServerState.Name##5  != SourceMove.Out##Name##5)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "5  != SourceMove.Out" #Name "5"))  return false; } } else break;\
    if (Name##6)  { if (ServerState.bReplicate##Name##6  && ServerState.Name##6  != SourceMove.Out##Name##6)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "6  != SourceMove.Out" #Name "6"))  return false; } } else break;\
    if (Name##7)  { if (ServerState.bReplicate##Name##7  && ServerState.Name##7  != SourceMove.Out##Name##7)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "7  != SourceMove.Out" #Name "7"))  return false; } } else break;\
    if (Name##8)  { if (ServerState.bReplicate##Name##8  && ServerState.Name##8  != SourceMove.Out##Name##8)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "8  != SourceMove.Out" #Name "8"))  return false; } } else break;\
    if (Name##9)  { if (ServerState.bReplicate##Name##9  && ServerState.Name##9  != SourceMove.Out##Name##9)  { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "9  != SourceMove.Out" #Name "9"))  return false; } } else break;\
    if (Name##10) { if (ServerState.bReplicate##Name##10 && ServerState.Name##10 != SourceMove.Out##Name##10) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "10 != SourceMove.Out" #Name "10")) return false; } } else break;\
    if (Name##11) { if (ServerState.bReplicate##Name##11 && ServerState.Name##11 != SourceMove.Out##Name##11) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "11 != SourceMove.Out" #Name "11")) return false; } } else break;\
    if (Name##12) { if (ServerState.bReplicate##Name##12 && ServerState.Name##12 != SourceMove.Out##Name##12) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "12 != SourceMove.Out" #Name "12")) return false; } } else break;\
    if (Name##13) { if (ServerState.bReplicate##Name##13 && ServerState.Name##13 != SourceMove.Out##Name##13) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "13 != SourceMove.Out" #Name "13")) return false; } } else break;\
    if (Name##14) { if (ServerState.bReplicate##Name##14 && ServerState.Name##14 != SourceMove.Out##Name##14) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "14 != SourceMove.Out" #Name "14")) return false; } } else break;\
    if (Name##15) { if (ServerState.bReplicate##Name##15 && ServerState.Name##15 != SourceMove.Out##Name##15) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "15 != SourceMove.Out" #Name "15")) return false; } } else break;\
    if (Name##16) { if (ServerState.bReplicate##Name##16 && ServerState.Name##16 != SourceMove.Out##Name##16) { GMC_LOG(Verbose, TEXT("Bound data deviates: ServerState_AutonomousProxy." #Name "16 != SourceMove.Out" #Name "16")) return false; } } else break;\
    return true;\
  }\
  while (false);

// Generic net serialization.
#define CALL_SerializeBoundDataGeneric(Name)\
  {\
    const bool bArIsSaving = Ar.IsSaving();\
    const bool bArIsLoading = Ar.IsLoading();\
    bool bForceFullSerialization = false;\
    if (bArIsSaving)\
    {\
      check(LastSerialized.Contains(CurrentTargetConnection))\
      bForceFullSerialization = !bOptimizeTraffic || LastSerialized[CurrentTargetConnection].bForceFullSerializationOnNextUpdate;\
    }\
    uint8 B = 0;\
    if (bArIsSaving) { if (bReplicate##Name##1)  { B = bForceFullSerialization ? 1 : Name##1  != LastSerialized[CurrentTargetConnection].Name##1;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##1;  LastSerialized[CurrentTargetConnection].Name##1  = Name##1;  } } } else if (bArIsLoading) { if (bReplicate##Name##1)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##1;  bReadNew##Name##1  = true; } else { bReadNew##Name##1  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##2)  { B = bForceFullSerialization ? 1 : Name##2  != LastSerialized[CurrentTargetConnection].Name##2;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##2;  LastSerialized[CurrentTargetConnection].Name##2  = Name##2;  } } } else if (bArIsLoading) { if (bReplicate##Name##2)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##2;  bReadNew##Name##2  = true; } else { bReadNew##Name##2  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##3)  { B = bForceFullSerialization ? 1 : Name##3  != LastSerialized[CurrentTargetConnection].Name##3;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##3;  LastSerialized[CurrentTargetConnection].Name##3  = Name##3;  } } } else if (bArIsLoading) { if (bReplicate##Name##3)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##3;  bReadNew##Name##3  = true; } else { bReadNew##Name##3  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##4)  { B = bForceFullSerialization ? 1 : Name##4  != LastSerialized[CurrentTargetConnection].Name##4;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##4;  LastSerialized[CurrentTargetConnection].Name##4  = Name##4;  } } } else if (bArIsLoading) { if (bReplicate##Name##4)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##4;  bReadNew##Name##4  = true; } else { bReadNew##Name##4  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##5)  { B = bForceFullSerialization ? 1 : Name##5  != LastSerialized[CurrentTargetConnection].Name##5;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##5;  LastSerialized[CurrentTargetConnection].Name##5  = Name##5;  } } } else if (bArIsLoading) { if (bReplicate##Name##5)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##5;  bReadNew##Name##5  = true; } else { bReadNew##Name##5  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##6)  { B = bForceFullSerialization ? 1 : Name##6  != LastSerialized[CurrentTargetConnection].Name##6;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##6;  LastSerialized[CurrentTargetConnection].Name##6  = Name##6;  } } } else if (bArIsLoading) { if (bReplicate##Name##6)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##6;  bReadNew##Name##6  = true; } else { bReadNew##Name##6  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##7)  { B = bForceFullSerialization ? 1 : Name##7  != LastSerialized[CurrentTargetConnection].Name##7;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##7;  LastSerialized[CurrentTargetConnection].Name##7  = Name##7;  } } } else if (bArIsLoading) { if (bReplicate##Name##7)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##7;  bReadNew##Name##7  = true; } else { bReadNew##Name##7  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##8)  { B = bForceFullSerialization ? 1 : Name##8  != LastSerialized[CurrentTargetConnection].Name##8;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##8;  LastSerialized[CurrentTargetConnection].Name##8  = Name##8;  } } } else if (bArIsLoading) { if (bReplicate##Name##8)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##8;  bReadNew##Name##8  = true; } else { bReadNew##Name##8  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##9)  { B = bForceFullSerialization ? 1 : Name##9  != LastSerialized[CurrentTargetConnection].Name##9;  Ar.SerializeBits(&B, 1); if (B) { Ar << Name##9;  LastSerialized[CurrentTargetConnection].Name##9  = Name##9;  } } } else if (bArIsLoading) { if (bReplicate##Name##9)  { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##9;  bReadNew##Name##9  = true; } else { bReadNew##Name##9  = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##10) { B = bForceFullSerialization ? 1 : Name##10 != LastSerialized[CurrentTargetConnection].Name##10; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##10; LastSerialized[CurrentTargetConnection].Name##10 = Name##10; } } } else if (bArIsLoading) { if (bReplicate##Name##10) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##10; bReadNew##Name##10 = true; } else { bReadNew##Name##10 = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##11) { B = bForceFullSerialization ? 1 : Name##11 != LastSerialized[CurrentTargetConnection].Name##11; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##11; LastSerialized[CurrentTargetConnection].Name##11 = Name##11; } } } else if (bArIsLoading) { if (bReplicate##Name##11) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##11; bReadNew##Name##11 = true; } else { bReadNew##Name##11 = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##12) { B = bForceFullSerialization ? 1 : Name##12 != LastSerialized[CurrentTargetConnection].Name##12; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##12; LastSerialized[CurrentTargetConnection].Name##12 = Name##12; } } } else if (bArIsLoading) { if (bReplicate##Name##12) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##12; bReadNew##Name##12 = true; } else { bReadNew##Name##12 = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##13) { B = bForceFullSerialization ? 1 : Name##13 != LastSerialized[CurrentTargetConnection].Name##13; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##13; LastSerialized[CurrentTargetConnection].Name##13 = Name##13; } } } else if (bArIsLoading) { if (bReplicate##Name##13) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##13; bReadNew##Name##13 = true; } else { bReadNew##Name##13 = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##14) { B = bForceFullSerialization ? 1 : Name##14 != LastSerialized[CurrentTargetConnection].Name##14; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##14; LastSerialized[CurrentTargetConnection].Name##14 = Name##14; } } } else if (bArIsLoading) { if (bReplicate##Name##14) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##14; bReadNew##Name##14 = true; } else { bReadNew##Name##14 = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##15) { B = bForceFullSerialization ? 1 : Name##15 != LastSerialized[CurrentTargetConnection].Name##15; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##15; LastSerialized[CurrentTargetConnection].Name##15 = Name##15; } } } else if (bArIsLoading) { if (bReplicate##Name##15) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##15; bReadNew##Name##15 = true; } else { bReadNew##Name##15 = false; } } }\
    if (bArIsSaving) { if (bReplicate##Name##16) { B = bForceFullSerialization ? 1 : Name##16 != LastSerialized[CurrentTargetConnection].Name##16; Ar.SerializeBits(&B, 1); if (B) { Ar << Name##16; LastSerialized[CurrentTargetConnection].Name##16 = Name##16; } } } else if (bArIsLoading) { if (bReplicate##Name##16) { Ar.SerializeBits(&B, 1); if (B) { Ar << Name##16; bReadNew##Name##16 = true; } else { bReadNew##Name##16 = false; } } }\
  }

// Variable definitions for input data. The functions for processing this data are implemented directly within the replication component as
// all the variable names are fixed.
// @see UGenMovementReplicationComponent::BindInputFlag
// @see UGenMovementReplicationComponent::UpdateBoundInputFlags
// @see UGenMovementReplicationComponent::AddTargetStateInputFlagsToInitState
// @see UGenMovementReplicationComponent::AddStartStateInputFlagsToInitState
// @see UGenMovementReplicationComponent::Server_CopyInputFlagsToServerState
// @see UGenMovementReplicationComponent::Server_ForceNetUpdateCheckInputFlags
// @see UGenMovementReplicationComponent::Server_ResetLockedInputFlags
// @see UGenMovementReplicationComponent::Client_EnqueueMoveCheckInputFlags
#define DEFINE_PREREPLICATED_INPUT_DATA()\
  bool* InputFlag1{nullptr};\
  bool* InputFlag2{nullptr};\
  bool* InputFlag3{nullptr};\
  bool* InputFlag4{nullptr};\
  bool* InputFlag5{nullptr};\
  bool* InputFlag6{nullptr};\
  bool* InputFlag7{nullptr};\
  bool* InputFlag8{nullptr};\
  bool* InputFlag9{nullptr};\
  bool* InputFlag10{nullptr};\
  bool* InputFlag11{nullptr};\
  bool* InputFlag12{nullptr};\
  bool* InputFlag13{nullptr};\
  bool* InputFlag14{nullptr};\
  bool* InputFlag15{nullptr};\
  bool* InputFlag16{nullptr};\
  bool bLockedInputFlag1{false};\
  bool bLockedInputFlag2{false};\
  bool bLockedInputFlag3{false};\
  bool bLockedInputFlag4{false};\
  bool bLockedInputFlag5{false};\
  bool bLockedInputFlag6{false};\
  bool bLockedInputFlag7{false};\
  bool bLockedInputFlag8{false};\
  bool bLockedInputFlag9{false};\
  bool bLockedInputFlag10{false};\
  bool bLockedInputFlag11{false};\
  bool bLockedInputFlag12{false};\
  bool bLockedInputFlag13{false};\
  bool bLockedInputFlag14{false};\
  bool bLockedInputFlag15{false};\
  bool bLockedInputFlag16{false};\
  float ResetLockTimerInputFlag1{0.f};\
  float ResetLockTimerInputFlag2{0.f};\
  float ResetLockTimerInputFlag3{0.f};\
  float ResetLockTimerInputFlag4{0.f};\
  float ResetLockTimerInputFlag5{0.f};\
  float ResetLockTimerInputFlag6{0.f};\
  float ResetLockTimerInputFlag7{0.f};\
  float ResetLockTimerInputFlag8{0.f};\
  float ResetLockTimerInputFlag9{0.f};\
  float ResetLockTimerInputFlag10{0.f};\
  float ResetLockTimerInputFlag11{0.f};\
  float ResetLockTimerInputFlag12{0.f};\
  float ResetLockTimerInputFlag13{0.f};\
  float ResetLockTimerInputFlag14{0.f};\
  float ResetLockTimerInputFlag15{0.f};\
  float ResetLockTimerInputFlag16{0.f};\
  float LockSetTimeInputFlag1{0.f};\
  float LockSetTimeInputFlag2{0.f};\
  float LockSetTimeInputFlag3{0.f};\
  float LockSetTimeInputFlag4{0.f};\
  float LockSetTimeInputFlag5{0.f};\
  float LockSetTimeInputFlag6{0.f};\
  float LockSetTimeInputFlag7{0.f};\
  float LockSetTimeInputFlag8{0.f};\
  float LockSetTimeInputFlag9{0.f};\
  float LockSetTimeInputFlag10{0.f};\
  float LockSetTimeInputFlag11{0.f};\
  float LockSetTimeInputFlag12{0.f};\
  float LockSetTimeInputFlag13{0.f};\
  float LockSetTimeInputFlag14{0.f};\
  float LockSetTimeInputFlag15{0.f};\
  float LockSetTimeInputFlag16{0.f};\
  bool bNoMoveCombineInputFlag1{false};\
  bool bNoMoveCombineInputFlag2{false};\
  bool bNoMoveCombineInputFlag3{false};\
  bool bNoMoveCombineInputFlag4{false};\
  bool bNoMoveCombineInputFlag5{false};\
  bool bNoMoveCombineInputFlag6{false};\
  bool bNoMoveCombineInputFlag7{false};\
  bool bNoMoveCombineInputFlag8{false};\
  bool bNoMoveCombineInputFlag9{false};\
  bool bNoMoveCombineInputFlag10{false};\
  bool bNoMoveCombineInputFlag11{false};\
  bool bNoMoveCombineInputFlag12{false};\
  bool bNoMoveCombineInputFlag13{false};\
  bool bNoMoveCombineInputFlag14{false};\
  bool bNoMoveCombineInputFlag15{false};\
  bool bNoMoveCombineInputFlag16{false};

//------------------------------------------------------------------------------------------------------------------------------------------
// Macro implementation: Custom serialization options for client-to-server replication.

// Place in the header of a derived class (private) and pass the appropriate arguments to implement the necessary functions. This will allow
// you to use a USTRUCT derived from FMove with custom serialization settings.
// TypeName: the type of your FMove derived struct.
// MemberName: the variable name of the class member that is of the derived struct's type.
// RPCName: the name of your custom send-moves-to-server RPC declared within the class.
// @note The reinterpret_cast here should be fine as long as no additional data members or virtual functions are added to the derived struct
// type.
#define IMPLEMENT_CUSTOM_SERIALIZATION_SETTINGS(TypeName, MemberName, RPCName)\
  TypeName MemberName;\
  FMove& LocalMove() override { return MemberName; }\
  const FMove& LocalMove() const override { return MemberName; }\
  void Client_SendMovesToServer() override {\
    check(sizeof TypeName == sizeof FMove && std::is_standard_layout<TypeName>::value && std::is_standard_layout<FMove>::value)\
    const TArray<TypeName>& PendingMoves = reinterpret_cast<const TArray<TypeName>&>(Client_GetPendingMoves());\
    RPCName(PendingMoves);\
  }\
  void RPCName##_Implementation(const TArray<TypeName>& RemoteMoves) {\
    check(sizeof TypeName == sizeof FMove && std::is_standard_layout<TypeName>::value && std::is_standard_layout<FMove>::value)\
    const TArray<FMove>& ReceivedMoves = reinterpret_cast<const TArray<FMove>&>(RemoteMoves);\
    Server_ProcessClientMoves(ReceivedMoves);\
  }\
  bool RPCName##_Validate(const TArray<TypeName>& RemoteMoves) {\
    return true;\
  }
