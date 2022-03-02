// Copyright 2022 Dominik Scherer. All Rights Reserved.
#pragma once

#include "GMC_PCH.h"
#include "GenPawn.h"
#include "PrereplicatedData.h"
#include "GenMovementReplicationComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGMCReplication, Log, All);

DECLARE_STATS_GROUP(TEXT("GMCReplicationComponent_Game"), STATGROUP_GMCReplicationComp, STATCAT_Advanced);

UENUM()
enum class ESizeQuantization : uint8
{
  Byte,
  Short,
  None,
};

UENUM()
enum class EDecimalQuantization : uint8
{
  RoundWholeNumber,
  RoundOneDecimal,
  RoundTwoDecimals,
  None,
};

USTRUCT(BlueprintType)
struct GMC_API FMove
{
  GENERATED_BODY()

	// The timestamp is important for the order and relevancy of moves. It is sent from the client to the server with every move
	// (uncompressed). A negative timestamp signifies that this is an invalid or uninitialized move e.g. a move created through the default
	// constructor. Such a move has no pawn associated with it but it can be used to store data temporarily or as an "empty" return value.
	// 时间戳对于移动的顺序和相关性很重要。 每次移动（未压缩）时，它都会从客户端发送到服务器。
	// 负时间戳表示这是无效或未初始化的移动，例如 通过默认构造函数创建的移动。
	// 这样的移动没有与之关联的棋子，但它可以用于临时存储数据或作为“空”返回值。
	float Timestamp{-1.f};

	// The delta time for the current move. This could be an accumulated value if no other input has changed since the last move and moves
	// have been combined. The delta time is not replicated, the server calculates the delta time of the client from the timestamps. Clients
	// do the same to ensure that the same value is used on both machines.
	// 当前移动的增量时间。 如果自上次移动后没有任何其他输入发生变化并且移动已合并，则这可能是一个累积值。
	// 增量时间不复制，服务器根据时间戳计算客户端的增量时间。 客户端执行相同操作以确保在两台机器上使用相同的值。
	float DeltaTime{0.f};

	// Used to refer to the In- and Out-variables of a move.
	// 用于指移动的输入和输出变量。
	enum EStateVars { Input, Output };

	// Input values. These are the starting values before move execution i.e. the input for the movement logic. The input vector is the
	// direction of movement received from the controller (e.g. from WASD or the left analog stick), the control rotation is the controller
	// view (e.g. from the mouse or the right analog stick). The rotation is the actor's root component orientation. The input flags are
	// boolean inputs that are usually used for triggerable abilities (e.g. sprinting or a dodge).
	// 输入值。 这些是移动执行之前的起始值，即移动逻辑的输入。
	// 输入向量是从控制器接收到的移动方向（例如，来自 WASD 或左模拟摇杆），控制旋转是控制器视图（例如，来自鼠标或右模拟摇杆）。
	// 旋转是actor的根组件方向。 输入标志是布尔输入，通常用于可触发的能力（例如冲刺或闪避）。
	FVector InputVector{0};
	bool bInputFlag1{false};
	bool bInputFlag2{false};
	bool bInputFlag3{false};
	bool bInputFlag4{false};
	bool bInputFlag5{false};
	bool bInputFlag6{false};
	bool bInputFlag7{false};
	bool bInputFlag8{false};
	bool bInputFlag9{false};
	bool bInputFlag10{false};
	bool bInputFlag11{false};
	bool bInputFlag12{false};
	bool bInputFlag13{false};
	bool bInputFlag14{false};
	bool bInputFlag15{false};
	bool bInputFlag16{false};
	FVector InVelocity{0};
	FVector InLocation{0};
	FRotator InRotation{0};
	FRotator InControlRotation{0};
	EInputMode InInputMode{0};

	// Output values. These are the results of the move execution i.e. the output of the movement logic. The server calculates those values
	// from the input as well, but may use the out location/rotation/control rotation (depending on the settings) sent by the client to
	// compare the local server results and determine if the client move was valid.
	// 输出值。 这些是移动执行的结果，即移动逻辑的输出。
	// 服务器也会根据输入计算这些值，但可能会使用客户端发送的输出位置/旋转/控制旋转（取决于设置）来比较本地服务器结果并确定客户端移动是否有效。
	FVector OutVelocity{0};
	FVector OutLocation{0};
	FRotator OutRotation{0};
	FRotator OutControlRotation{0};
	EInputMode OutInputMode{0};

	// Used on the client to indicate that a value has changed and needs to be serialized, and on the server to indicate that a new value was
	// received and deserialized.
	// 用在客户端表示某个值发生了变化需要序列化，用在服务端表示接收到一个新的值并反序列化。
	bool bHasNewInputVectorX{false};
	bool bHasNewInputVectorY{false};
	bool bHasNewInputVectorZ{false};
	bool bHasNewOutVelocity{false};
	bool bHasNewOutLocation{false};
	bool bHasNewOutRotationRoll{false};
	bool bHasNewOutRotationPitch{false};
	bool bHasNewOutRotationYaw{false};
	bool bHasNewOutControlRotationRoll{false};
	bool bHasNewOutControlRotationPitch{false};
	bool bHasNewOutControlRotationYaw{false};

	// Serialization and compression options. Cannot be changed at runtime because moves are sent via RPC argument from client to server.
	// Custom settings can be implemented for individual classes by implementing a derived struct and reconfiguring the replication options
	// in the constructor.
	// @attention It is strongly recommended to use the default quantization levels. These are not only used to compress the data when sending
	// the move to the server, they affect the local pawn state as well. After a move was executed the autonomous proxy pawn is set to a
	// quantized state based on the configured settings, and if the compression is too strong this might be noticable while playing.
	// @attention Never use byte compression. It is not accurate enough to get consistent quantization results across client and server and
	// may trigger erroneous replays.
	// @attention The out velocity is currently not used anywhere on the server and should not be replicated. The velocity is still quantized
	// locally on the client with the level that is set within the autonomous proxy server state, the quantization level that is set here is
	// not used either.
	// @attention When setting the number of serialized input flags, keep in mind that input flags can be skipped during binding (e.g. input
	// flags 1 and 3 could be bound, but not 2). However, the value of NumSerializedInputFlags will cause the specified amount to be
	// serialized in order (e.g. setting the value 3 will serialize input flags 1, 2 and 3), so depending on how the input flags are bound
	// some may not be replicated even if the number of bound flags matches the value of NumSerializedInputFlags. Only change this property
	// from the default max value if you understand the implications.

	// 序列化和压缩选项。无法在运行时更改，因为移动是通过 RPC 参数从客户端发送到服务器的。
	// 通过实现派生结构并在构造函数中重新配置复制选项，可以为各个类实现自定义设置。

	// @attention 强烈建议使用默认量化级别。这些不仅用于在将移动发送到服务器时压缩数据，它们也会影响本地 pawn 状态。
	// 执行移动后，自主代理 pawn 根据配置设置为量化状态，如果压缩太强，则在播放时可能会注意到这一点。

	// @attention 永远不要使用字节压缩。它不够准确，无法在客户端和服务器之间获得一致的量化结果，并且可能会触发错误的重放。

	// @attention 输出速度当前在服务器上的任何地方都没有使用，不应该被复制。
	// 速度仍然在客户端本地量化，级别在自治代理服务器状态内设置，此处设置的量化级别也不使用。

	// @attention 设置序列化输入标志的数量时，请记住在绑定期间可以跳过输入标志（例如，可以绑定输入标志 1 和 3，但不能绑定 2）。
	// 但是，NumSerializedInputFlags 的值将导致指定数量按顺序序列化（例如，将值设置为 3 将序列化输入标志 1、2 和 3），
	// 因此根据输入标志的绑定方式，即使绑定标志的数量与 NumSerializedInputFlags 的值匹配。如果您了解其含义，请仅将此属性从默认最大值更改。
	ESizeQuantization InputVectorQuantize{ESizeQuantization::Short};
	EDecimalQuantization OutVelocityQuantize{EDecimalQuantization::RoundTwoDecimals}; // Not used.
	EDecimalQuantization OutLocationQuantize{EDecimalQuantization::RoundTwoDecimals};
	ESizeQuantization OutRotationQuantize{ESizeQuantization::Short};
	ESizeQuantization OutControlRotationQuantize{ESizeQuantization::Short};
	bool bSerializeInputVectorX{true};
	bool bSerializeInputVectorY{true};
	bool bSerializeInputVectorZ{true};
	bool bSerializeOutVelocity{false}; // Not used, keep disabled.
	bool bSerializeOutLocation{true};
	bool bSerializeOutRotationRoll{true};
	bool bSerializeOutRotationPitch{true};
	bool bSerializeOutRotationYaw{true};
	bool bSerializeOutControlRotationRoll{true};
	bool bSerializeOutControlRotationPitch{true};
	bool bSerializeOutControlRotationYaw{true};
	int32 NumSerializedInputFlags{16};

	FMove() = default;
	FMove(
	float Timestamp,
	float DeltaTime = 0.f,
	FVector InputVector = FVector{0},
	bool bInputFlag1 = false,
	bool bInputFlag2 = false,
	bool bInputFlag3 = false,
	bool bInputFlag4 = false,
	bool bInputFlag5 = false,
	bool bInputFlag6 = false,
	bool bInputFlag7 = false,
	bool bInputFlag8 = false,
	bool bInputFlag9 = false,
	bool bInputFlag10 = false,
	bool bInputFlag11 = false,
	bool bInputFlag12 = false,
	bool bInputFlag13 = false,
	bool bInputFlag14 = false,
	bool bInputFlag15 = false,
	bool bInputFlag16 = false,
	FVector InVelocity = FVector{0},
	FVector InLocation = FVector{0},
	FRotator InRotation = FRotator{0},
	FRotator InControlRotation = FRotator{0},
	EInputMode InInputMode = EInputMode::None
	);

	bool IsValid() const { return Timestamp >= 0.f; }
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	bool SerializeInputVector(FArchive& Ar);
	void SerializeInputFlags(FArchive& Ar);
	bool SerializeOutVelocity(FArchive& Ar);
	bool SerializeOutLocation(FArchive& Ar);
	void SerializeOutRotation(FArchive& Ar);
	void SerializeOutControlRotation(FArchive& Ar);
	void QuantizeInputVector();
	void QuantizeOutVelocity();
	void QuantizeOutLocation();
	void QuantizeOutRotation();
	void QuantizeOutControlRotation();
	DEFINE_PREREPLICATED_DATA_FMOVE()
};

template<>
struct TStructOpsTypeTraits<FMove> : public TStructOpsTypeTraitsBase2<FMove>
{
  enum
  {
    WithNetSerializer = true
  };
};

USTRUCT()
struct GMC_API FStateReduced
{
  GENERATED_BODY()

  bool bReplicatedToSimulatedProxy{false};
  bool bWasNetRelevantLastFrame{false};
  bool bForceFullSerializationOnNextUpdate{false};

  FVector Velocity{0};
  FVector Location{0};
  float RotationRoll{0.f};
  float RotationPitch{0.f};
  float RotationYaw{0.f};
  float ControlRotationRoll{0.f};
  float ControlRotationPitch{0.f};
  float ControlRotationYaw{0.f};
  EInputMode InputMode{0};
  DEFINE_PREREPLICATED_DATA_FSTATEREDUCED()
};

USTRUCT(BlueprintType)
struct GMC_API FState
{
  GENERATED_BODY()

	// The timestamp is important for the order and relevancy of states. It is sent from the server to the client with every replication
	// update (uncompressed). A negative timestamp signifies that this is an invalid or uninitialized state e.g. a state created through the
	// default constructor. Such a state has no pawn associated with it but it can be used to store data temporarily or as an "empty" return
	// value.
	// 时间戳对于状态的顺序和相关性很重要。 每次复制更新（未压缩）时，它都会从服务器发送到客户端。
	// 负时间戳表示这是无效或未初始化的状态，例如 通过默认构造函数创建的状态。 这种状态没有与之关联的pawn，但它可以用于临时存储数据或作为“空”返回值。
	UPROPERTY(Transient, BlueprintReadOnly, Category = "General Movement Component")
	float Timestamp{-1.f};

	UPROPERTY()
	// The owning pawn of this state. This is mostly relevant for the server states of the replication component. Assigned when the pawn is
	// spawned (@see UGenMovementReplicationComponent::SetUpdatedComponent).
	// 该状态的拥有棋子。这主要与复制组件的服务器状态相关。在生成 pawn 时分配（@see UGenMovementReplicationComponent::SetUpdatedComponent）。
	AGenPawn* Owner{nullptr};

	// For the server state members of the replication component, this will hold the net role of the recipient (either ROLE_AutonomousProxy or
	// ROLE_SimulatedProxy).
	// 对于复制组件的服务器状态成员，这将持有接收者的网络角色（ROLE_AutonomousProxy 或 ROLE_SimulatedProxy）。
	ENetRole RecipientRole{ROLE_None};

	// Whether we should optimize bandwidth usage when replicating the server state. This flag will be set and updated automatically by the
	// server based on the value set in the replication component (@see UGenMovementReplicationComponent::bOptimizeTraffic), do not modify
	// directly.
	// 我们是否应该在复制服务器状态时优化带宽使用。
	// 此标志将由服务器根据复制组件中设置的值自动设置和更新（@see UGenMovementReplicationComponent::bOptimizeTraffic），请勿直接修改。
	bool bOptimizeTraffic{false};

	// Set by the server to inform the client whether the replicated server state contains the full set of replicated data. Mainly used in the
	// context of the client replay. When a client move was valid, the autonomous proxy usually only receives the velocity (plus the input
	// mode and potentially bound data) from the server, but if a client source move (the move with the same timestamp as the replicated
	// server state) was not valid, the server sends the full batch of replicated state data (velocity, location, actor rotation, control
	// rotation) so the client can replay. Simulated proxies always receive the full batch of replication data meaning this property will
	// always be true when replicating to a simulated proxy (@see NetSerialize).
	// @attention Must be initialized with "true". The value is only ever changed to "false" on the server for the autonomous proxy server
	// state, and simulated proxies don't even need to receive this property as it is never changed from the initial value "true" for those
	// pawns.
	// 由服务器设置，通知客户端复制的服务器状态是否包含完整的复制数据集。主要用在客户端重放的上下文中。
	// 当客户端移动有效时，自治代理通常只从服务器接收速度（加上输入模式和可能绑定的数据），
	// 但如果客户端源移动（与复制的服务器状态具有相同时间戳的移动）不是有效时，服务器会发送整批复制的状态数据（速度、位置、actor 旋转、Control旋转），
	// 以便客户端可以重播。 模拟代理始终接收整批复制数据，这意味着在复制到模拟代理时此属性将始终为真（@see NetSerialize）。
	bool bContainsFullRepBatch{true};

	// Replicated state values. An FState saves the current state of a pawn. As such it does not have input and output values like a move.
	// When used for server state replication however, the properties are saved after the movement was executed on the server so they are the
	// equivalent of the out values of a move. States are used for the local smoothing of remotely controlled pawns and as a way for the
	// autonomous proxy to validate its own state and stay in sync with the server.
	// 复制的状态值。 FState 保存 pawn 的当前状态。 因此，它不像移动那样具有输入和输出值。
	// 但是，当用于服务器状态复制时，属性会在服务器上执行移动后保存，因此它们相当于移动的输出值。
	// 状态用于远程控制 pawn 的本地平滑，并作为自治代理验证其自身状态并与服务器保持同步的一种方式。
	UPROPERTY(Transient, BlueprintReadOnly, Category = "General Movement Component")
	FVector Velocity{0};
	UPROPERTY(Transient, BlueprintReadOnly, Category = "General Movement Component")
	FVector Location{0};
	UPROPERTY(Transient, BlueprintReadOnly, Category = "General Movement Component")
	FRotator Rotation{0};
	UPROPERTY(Transient, BlueprintReadOnly, Category = "General Movement Component")
	FRotator ControlRotation{0};
	UPROPERTY(Transient, BlueprintReadOnly, Category = "General Movement Component")
	EInputMode InputMode{0};
	bool bInputFlag1{false};
	bool bInputFlag2{false};
	bool bInputFlag3{false};
	bool bInputFlag4{false};
	bool bInputFlag5{false};
	bool bInputFlag6{false};
	bool bInputFlag7{false};
	bool bInputFlag8{false};
	bool bInputFlag9{false};
	bool bInputFlag10{false};
	bool bInputFlag11{false};
	bool bInputFlag12{false};
	bool bInputFlag13{false};
	bool bInputFlag14{false};
	bool bInputFlag15{false};
	bool bInputFlag16{false};

	// Used by @see UGenMovementReplicationComponent::Server_SwapStateBuffer to buffer the relative location and rotation of the simulated
	// root component (@see UGenMovementReplicationComponent::SimulatedRootComponent) before and after remote move execution on the server.
	// @see UGenMovementReplicationComponent::Server_SwapStateBuffer 用于缓冲模拟根组件（@see UGenMovementReplicationComponent::SimulatedRootComponent）在服务器上执行远程移动之前和之后的相对位置和旋转。
	FVector SimulatedRootRelativeLocation{0};
	FRotator SimulatedRootRelativeRotation{0};

	// Quantization levels are set from the replication component options in @see UGenMovementReplicationComponent::PostInitProperties and are
	// always set to match between the autonomous and simulated proxy server states.
	// @attention Byte compression should never be used since the replicated value cannot be quantized back reliably to the original one by
	// the recipient.
	// @attention It is strongly recommended to use the default quantization levels. These are not only used to compress the data for
	// replication, they affect the local pawn state as well. After a move was executed the pawn is set to a quantized state based on the
	// configured settings, and if the compression is too strong this might be noticable while playing.
	// 量化级别从@see UGenMovementReplicationComponent::PostInitProperties 中的复制组件选项设置，并且始终设置为在自主和模拟代理服务器状态之间匹配。
	// @attention 永远不应该使用字节压缩，因为复制的值不能被接收者可靠地量化回原始值。
	// @attention 强烈建议使用默认量化级别。 这些不仅用于压缩数据以进行复制，它们也会影响本地 pawn 状态。
	// 执行移动后，pawn 会根据配置的设置设置为量化状态，如果压缩太强，在播放时可能会很明显。
	EDecimalQuantization LocationQuantize{EDecimalQuantization::RoundTwoDecimals};
	EDecimalQuantization VelocityQuantize{EDecimalQuantization::RoundTwoDecimals};
	ESizeQuantization RotationQuantize{ESizeQuantization::Short};
	ESizeQuantization ControlRotationQuantize{ESizeQuantization::Short};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Timestamp", Category = "Replication", meta =
		(Tooltip = "Whether the timestamp of the server state should be replicated.\n是否应复制服务器状态的时间戳."))
	bool bSerializeTimestamp{true};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Move Validation", Category = "Replication", meta =
	  (Tooltip = "Whether the result of the client move validation should be replicated. Should always be replicated to the autonomous proxy, but never to simulated proxies.\n是否应复制客户端移动验证的结果. 应始终复制到自治代理, 但永远不要复制到模拟代理."))
	bool bSerializeMoveValidation{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Location", Category = "Replication", meta =
		(Tooltip = "Whether the location of the pawn should be replicated to the client. ATTENTION: When disabled, the location cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的位置复制到客户端. 注意：禁用时, 不能再在复制的Tick函数内本地更改位置."))
	bool bSerializeLocation{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Velocity", Category = "Replication", meta =
		(Tooltip = "Whether the velocity of the pawn should be replicated to the client. ATTENTION: When disabled, the velocity cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的速度复制到客户端. 注意: 禁用后, 无法再在复制的Tick函数内本地更改速度."))
	bool bSerializeVelocity{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Rotation Roll", Category = "Replication", meta =
		(Tooltip = "Whether the rotation roll of the pawn should be replicated to the client. ATTENTION: When disabled, the rotation roll cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Rotation Roll复制到客户端. 注意: 禁用时,无法再在复制的Tick函数内本地更改Rotation Roll."))
	bool bSerializeRotationRoll{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Rotation Pitch", Category = "Replication", meta =
		(Tooltip = "Whether the rotation pitch of the pawn should be replicated to the client. ATTENTION: When disabled, the rotation pitch cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Rotation Pitch复制到客户端. 注意: 禁用时, 无法再在复制的Tick函数内本地更改Rotation Pitch."))
	bool bSerializeRotationPitch{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Rotation Yaw", Category = "Replication", meta =
		(Tooltip = "Whether the rotation yaw of the pawn should be replicated to the client. ATTENTION: When disabled, the rotation yaw cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Rotation Yaw复制到客户端. 注意: 禁用时, 无法再在复制的Tick函数内本地更改Rotation Yaw."))
	bool bSerializeRotationYaw{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Control Rotation Roll", Category = "Replication", meta =
		(Tooltip = "Whether the control rotation roll of the pawn should be replicated to the client. ATTENTION: When disabled, the control rotation roll cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Control Rotation Roll复制到客户端. 注意: 禁用时, 无法再在复制的Tick函数内本地更改Control Rotation Roll."))
	bool bSerializeControlRotationRoll{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Control Rotation Pitch", Category = "Replication", meta =
		(Tooltip = "Whether the control rotation pitch of the pawn should be replicated to the client. ATTENTION: When disabled, the rotation pitch cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Control Rotation Pitch复制到客户端. 注意: 禁用时, 无法再在复制的Tick函数内本地更改的Control Rotation Pitch."))
	bool bSerializeControlRotationPitch{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Control Rotation Yaw", Category = "Replication", meta =
		(Tooltip = "Whether the control rotation yaw of the pawn should be replicated to the client. ATTENTION: When disabled, the rotation yaw cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Control Rotation Yaw复制到客户端. 注意: 禁用时, 无法再在复制的Tick函数内本地更改Control Rotation Yaw."))
	bool bSerializeControlRotationYaw{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Input Mode", Category = "Replication", meta =
		(Tooltip = "Whether the input mode of the pawn should be replicated to the client. ATTENTION: When disabled, the input mode cannot be changed locally anymore inside the replicated tick function.\n是否应将 pawn 的Input Mode复制到客户端. 注意: 禁用时, 无法再在复制的Tick函数内本地更改Input Mode."))
	bool bSerializeInputMode{true};
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, DisplayName = "Replicate Bound Data", Category = "Replication", meta =
		(Tooltip = "Whether variables that were bound to pre-replicated data should be replicated.\n是否应该复制绑定到预复制数据的变量."))
	bool bSerializeBoundData{true};

  // Flags to keep track of which bound input flags should be replicated to simulated proxies. Input flags are never replicated to the
  // autonomous proxy.
  bool bReplicateInputFlag1{false};
  bool bReplicateInputFlag2{false};
  bool bReplicateInputFlag3{false};
  bool bReplicateInputFlag4{false};
  bool bReplicateInputFlag5{false};
  bool bReplicateInputFlag6{false};
  bool bReplicateInputFlag7{false};
  bool bReplicateInputFlag8{false};
  bool bReplicateInputFlag9{false};
  bool bReplicateInputFlag10{false};
  bool bReplicateInputFlag11{false};
  bool bReplicateInputFlag12{false};
  bool bReplicateInputFlag13{false};
  bool bReplicateInputFlag14{false};
  bool bReplicateInputFlag15{false};
  bool bReplicateInputFlag16{false};

  UPROPERTY()
  // Designates the current target client connection during net serialization.
  APlayerController* CurrentTargetConnection{nullptr};

  UPROPERTY(NotReplicated/*Stop UHT from complaining about TMaps not being supported for replication.*/)
  // Used on the server to determine if a value has changed since the last replication update. Values are only replicated fully if they have
  // changed since the last serialization to save bandwidth. If a value has not changed only one bit is sent to indicate to the client that
  // the value from the last update should be used again. We map server-client connections (represented through player controllers on the
  // server) to state data for each pawn. For autonomous proxies there's only one connection to replicate to, but one server pawn can
  // potentially replicate to multiple simulated proxies so we need to manage the last serialized data for each connection individually.
  TMap<APlayerController*, FStateReduced> LastSerialized;

  // Used on the client to discern if a new value was received.
  bool bReadNewVelocity{false};
  bool bReadNewLocation{false};
  bool bReadNewRotationRoll{false};
  bool bReadNewRotationPitch{false};
  bool bReadNewRotationYaw{false};
  bool bReadNewControlRotationRoll{false};
  bool bReadNewControlRotationPitch{false};
  bool bReadNewControlRotationYaw{false};
  bool bReadNewInputMode{false};

  FState() = default;

  bool IsValid() const { return Timestamp >= 0.f; }
  bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
  bool SerializeLocation(FArchive& Ar);
  bool SerializeVelocity(FArchive& Ar);
  void SerializeRotation(FArchive& Ar);
  void SerializeControlRotation(FArchive& Ar);
  void SerializeInputFlags(FArchive& Ar);
  void SerializeBoundData(FArchive& Ar);
  void SerializeMoveValidation(FArchive& Ar);
  void SerializeInputMode(FArchive& Ar);
  void QuantizeVelocity();
  void QuantizeLocation();
  void QuantizeRotation();
  void QuantizeControlRotation();
  DEFINE_PREREPLICATED_DATA_FSTATE()

  // Specialized net serialization functions for bound data.
  void SerializeBoolTypes(FArchive& Ar);
  void SerializeHalfByteTypes(FArchive& Ar);
  void SerializeVectorTypes(FArchive& Ar);
  void SerializeNormalTypes(FArchive& Ar);
  void SerializeRotatorTypes(FArchive& Ar);
};

template <>struct TStructOpsTypeTraits<FState> : public TStructOpsTypeTraitsBase2<FState>
{
  enum
  {
    WithNetSerializer = true
  };
};

UENUM(BlueprintType)
enum class EImmediateContext : uint8
{
  NoContextInformation UMETA(DisplayName = "NoContextInformation"),
  UnSmoothingRemoteListenServerPawn UMETA(DisplayName = "UnSmoothingRemoteListenServerPawn"),
  ReSmoothingRemoteListenServerPawn UMETA(DisplayName = "ReSmoothingRemoteListenServerPawn"),
  RestoringRolledBackServerPawn UMETA(DisplayName = "RestoringRolledBackServerPawn"),
  LocalServerPawnExecutingMove UMETA(DisplayName = "LocalServerPawnExecutingMove"),
  RemoteServerPawnExecutingMove UMETA(DisplayName = "RemoteServerPawnExecutingMove"),
  LocalClientPawnExecutingReplicatedMove UMETA(DisplayName = "LocalClientPawnExecutingReplicatedMove"),
  LocalClientPawnExecutingDiscardedMove UMETA(DisplayName = "LocalClientPawnExecutingDiscardedMove"),
  LocalClientPawnReplayingMove UMETA(DisplayName = "LocalClientPawnReplayingMove"),
  MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ESimulatedContext : uint8
{
  NoContextInformation UMETA(DisplayName = "NoContextInformation"),
  SmoothingRemoteListenServerPawn UMETA(DisplayName = "SmoothingRemoteListenServerPawn"),
  SmoothingSimulatedProxy UMETA(DisplayName = "SmoothingSimulatedProxy"),
  RollingBackServerPawn UMETA(DisplayName = "RollingBackServerPawn"),
  RollingBackClientPawn UMETA(DisplayName = "RollingBackClientPawn"),
  RestoringRolledBackClientPawn UMETA(DisplayName = "RestoringRolledBackClientPawn"),
  MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EInterpolationMethod : uint8
{
  None UMETA(DisplayName = "None"),
  Linear UMETA(DisplayName = "Linear"),
  Cubic UMETA(DisplayName = "Cubic"),
  Custom1 UMETA(DisplayName = "Custom1"),
  Custom2 UMETA(DisplayName = "Custom2"),
  Custom3 UMETA(DisplayName = "Custom3"),
  Custom4 UMETA(DisplayName = "Custom4"),
  MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ENetworkPreset : uint8
{
  LAN UMETA(DisplayName = "LAN", ToolTip = "Local network with no latency and ideal network conditions. 没有延迟和理想网络条件的本地网络。"),
  Competitive UMETA(DisplayName = "Competitive", ToolTip = "Excellent network conditions with wired connections, low latencies (ping < 60 ms), minimal jitter and no packet loss. 具有有线连接、低延迟（ping < 60 ms）、最小抖动和无数据包丢失的出色网络条件。"),
  Regular UMETA(DisplayName = "Regular", ToolTip = "Average network conditions with stable connections, moderate latencies (ping < 100 ms), little jitter and tolerable packet loss (< 2%). 平均网络条件，连接稳定、延迟适中（ping < 100 毫秒）、抖动小和可容忍的丢包率（< 2%）。"),
  LowEnd UMETA(DisplayName = "Low-end", ToolTip = "Poor network conditions with potentially unstable connections, high latencies (up to 200 ms), noticeable jitter and serious packet loss (up to 5%). 网络状况不佳，可能存在不稳定的连接、高延迟（高达 200 毫秒）、明显的抖动和严重的数据包丢失（高达 5%）。"),
  Custom UMETA(DisplayName = "Custom", ToolTip = "User-defined configuration. 用户定义的配置。"),
  MAX UMETA(Hidden),
};

/// Synchronises location, actor rotation, control rotation and velocity across server and clients for any owning actor. Subclasses can
/// implement replicated movement logic by binding new variables to special data members, which integrates them automatically into the
/// client-replay and the interpolation algorithm.
UCLASS(ABSTRACT, ClassGroup = "Movement", BlueprintType, NotBlueprintable)
class GMC_API UGenMovementReplicationComponent : public UPawnMovementComponent
{
  GENERATED_BODY()
  friend class AGenPlayerController;

public:

  UGenMovementReplicationComponent();
  void PostInitProperties() override;
  void SetUpdatedComponent(USceneComponent* NewUpdatedComponent);
  void BeginPlay() override;
  void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  void EndPlay(EEndPlayReason::Type EndPlayReason) override;
  void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;
  void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITOR

  bool CanEditChange(const FProperty* InProperty) const override;
  void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

#pragma region Network Profiles

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Networking")
  /// Choose a preset that fits the network conditions that you target. This will load predefined values that are appropriate for the
  /// setting outlined in the preset description. To make the loaded values editable change the profile to "Custom". The replication options
  /// are always editable.
  /// WARNING: Selecting any profile other than "Custom" will overwrite all current settings within the "Networking" category (including
  /// those in the "Replication" section).
  /// 选择适合您定位的网络条件的预设。 这将加载适用于预设描述中概述的设置的预定义值。
  /// 要使加载的值可编辑，请将配置文件更改为“自定义”。 复制选项始终是可编辑的。
  /// 警告：选择“自定义”以外的任何配置文件将覆盖“Networking”类别中的所有当前设置（包括“Replication”部分中的设置）。
  ENetworkPreset NetworkPreset{ENetworkPreset::LAN};

  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void LoadNetworkPreset(ENetworkPreset PresetToLoad);
  virtual void LoadServerStateReplicationPreset(ENetRole RecipientRole);

#pragma endregion

  /// Minimum delta time for calculations to avoid potential divide-by-zero.
  static constexpr float MIN_DELTA_TIME = 1e-6f;
  /// The maximum value for a component of the directional input vector.
  static constexpr int32 MAX_INPUT = 1;

#pragma region Utility

  /// Determines whether a float value has changed. Values are only serialized again if they have changed since the last time they were
  /// serialized.
  ///
  /// @param        CurrentValue     The current value.
  /// @param        LastSentValue    The last value that was serialized. Overwritten with the current value if it has changed.
  /// @param        Tolerance        The tolerance for the comparison.
  /// @returns      bool             True if the value has changed, false otherwise.
  static bool HasValueChanged(float CurrentValue, float& LastSentValue, float Tolerance);

  /// Overload for vectors.
  ///
  /// @param        CurrentValue     The current vector.
  /// @param        LastSentValue    The last vector that was serialized. Overwritten with the current vector if it has changed.
  /// @param        Tolerance        The tolerance for the comparison.
  /// @returns      bool             True if the vector has changed, false otherwise.
  static bool HasValueChanged(const FVector& CurrentValue, FVector& LastSentValue, float Tolerance);

  /// Determines the compare tolerance from the passed quantization level for decimal quantization. Used mainly for vectors.
  ///
  /// @param        QuantizationLevel    The decimal quantization level.
  /// @returns      float                The compare tolerance that guarantees accurate results for the given quantization level.
  static float GetCompareTolerance(EDecimalQuantization QuantizationLevel);

  /// Determines the compare tolerance for rotators with size quantization. Beware that rotators may need to be normalized before comparing
  /// them.
  ///
  /// @param        QuantizationLevel    The size quantization level.
  /// @returns      float                The compare tolerance that guarantees accurate results for the given quantization level.
  static float GetCompareToleranceRotator(ESizeQuantization QuantizationLevel);

  /// Determines the compare tolerance for vectors with size quantization and a maximum value of 1 per component. Used for the input vector
  /// which is clamped to have a maximum magnitude of 1. This means that it can retain fairly high accuracy even when compressed.
  ///
  /// @param        QuantizationLevel    The size quantization level.
  /// @returns      float                The compare tolerance that guarantees accurate results for the given quantization level.
  static float GetCompareToleranceVectorMax1(ESizeQuantization QuantizationLevel);

  /// Values that were not deserialized (because the option to replicate them is disabled) are marked with NaN values by the receiver of the
  /// update. This means a value has to be checked if before it can be used by the receiver.
  ///
  /// @param        UnpackedValue    The float that was unpacked by the receiver and that needs to be checked.
  /// @returns      bool             True is the value is valid (not NaN), false otherwise.
  static bool Rep_IsValid(const float& UnpackedValue);

  /// Overload for vectors.
  ///
  /// @param        UnpackedVector    The vector that was unpacked by the receiver and that needs to be checked.
  /// @returns      bool              True is the vector is valid (does not contain any NaN values), false otherwise.
  static bool Rep_IsValid(const FVector& UnpackedVector);

  /// Overload for rotators.
  ///
  /// @param        UnpackedRotator    The rotator that was unpacked by the receiver and that needs to be checked.
  /// @returns      bool               True is the rotator is valid (does not contain any NaN values), false otherwise.
  static bool Rep_IsValid(const FRotator& UnpackedRotator);

  /// Lets the receiver mark a value that was not replicated with NaN.
  ///
  /// @param        NonSerializedValue    The float that was not received/replicated and needs to be marked.
  /// @returns      void
  static void Rep_SetInvalid(float& NonSerializedValue);

  /// Overload for vectors.
  ///
  /// @param        NonSerializedVector    The vector that was not received/replicated and needs to be marked.
  /// @returns      void
  static void Rep_SetInvalid(FVector& NonSerializedVector);

  /// Overload for rotators.
  ///
  /// @param        NonSerializedRotator    The rotator that was not received/replicated and needs to be marked.
  /// @returns      void
  static void Rep_SetInvalid(FRotator& NonSerializedRotator);

  /// Returns a vector that consists of the valid (not NaN) components of the passed unpacked vector and, if there were NaN values in that
  /// vector, the components of the completive vector in place of the NaN values. The completive vector must not contain NaN values.
  ///
  /// @param        UnpackedVector      The vector that was unpacked by the receiver which may contain NaN values.
  /// @param        CompletiveVector    The vector from which to take the component values to replace the NaN values.
  /// @returns      FVector             A valid vector which may be a mixture of the components of the two passed vectors.
  static FVector GetValidVector(const FVector& UnpackedVector, const FVector& CompletiveVector);

  /// Returns a valid rotator, @see GetValidVector.
  ///
  /// @param        UnpackedRotator      The rotator that was unpacked by the receiver which may contain NaN values.
  /// @param        CompletiveRotator    The rotator from which to take the component values to replace the NaN values.
  /// @returns      FRotator             A valid rotator which may be a mixture of the components of the two passed rotators.
  static FRotator GetValidRotator(const FRotator& UnpackedRotator, const FRotator& CompletiveRotator);

  /// Returns a valid input vector, @see GetValidVector.
  ///
  /// @param        UnpackedRotator      The input vector that was unpacked by the receiver which may contain NaN values.
  /// @param        CompletiveRotator    The vector from which to take the component values to replace the NaN values.
  /// @returns      FRotator             A valid vector which may be a mixture of the components of the two passed vectors.
  static FVector GetValidInputVector(const FVector& UnpackedInputVector, const FVector& CompletiveInputVector = FVector{0});

  /// Writes a series of boolean values to a byte as bit-mask. When replicating several bools it is often more bandwidth efficient to pack
  /// them into a byte.
  ///
  /// @param        OutByte    The masked byte.
  /// @param        InBit1     The value bit 1 should have.
  /// @param        InBit2     The value bit 2 should have.
  /// @param        InBit3     The value bit 3 should have.
  /// @param        InBit4     The value bit 4 should have.
  /// @param        InBit5     The value bit 5 should have.
  /// @param        InBit6     The value bit 6 should have.
  /// @param        InBit7     The value bit 7 should have.
  /// @param        InBit8     The value bit 8 should have.
  /// @returns      void
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static void PackBoolsToByte(
    uint8& OutByte,
    bool InBit1 = false,
    bool InBit2 = false,
    bool InBit3 = false,
    bool InBit4 = false,
    bool InBit5 = false,
    bool InBit6 = false,
    bool InBit7 = false,
    bool InBit8 = false
  );

  /// Reads a byte as bit-mask of boolean values. When replicating several bools it is often more bandwidth efficient to pack them into a
  /// byte.
  ///
  /// @param        InByte     The byte to be read.
  /// @param        OutBit1    The value of bit 1.
  /// @param        OutBit2    The value of bit 2.
  /// @param        OutBit3    The value of bit 3.
  /// @param        OutBit4    The value of bit 4.
  /// @param        OutBit5    The value of bit 5.
  /// @param        OutBit6    The value of bit 6.
  /// @param        OutBit7    The value of bit 7.
  /// @param        OutBit8    The value of bit 8.
  /// @returns      void
  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "General Movement Component")
  static void UnpackBoolsFromByte(
    const uint8& InByte,
    bool& OutBit1,
    bool& OutBit2,
    bool& OutBit3,
    bool& OutBit4,
    bool& OutBit5,
    bool& OutBit6,
    bool& OutBit7,
    bool& OutBit8
  );

#pragma endregion

protected:

  /// Bind members that should be replicated here by using the binding-function for the respective variable type (e.g. BindBool, BindFloat,
  /// BindInt, etc.). All binding must be fully set up before any replication happens and bindings must always match across server and
  /// client, meaning you should not bind data anywhere else but within this function. Once bound, variables cannot be unbound.
  /// 通过使用相应变量类型（例如BindBool、BindFloat、BindInt等）的绑定函数，在此处复制的绑定成员。在进行任何复制之前，必须完全设置所有绑定，
  /// 并且绑定必须始终在服务器和客户端之间匹配，这意味着您不应该在除此函数之外的任何地方绑定数据。一旦绑定，变量就无法解除绑定。
  ///
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  void BindReplicationData();
  virtual void BindReplicationData_Implementation() {}

  /// Replicated movement tick meant to be overridden by derived classes. This is supposed to contain the logic for moving the pawn and
  /// calculating a new velocity every frame. The implementation must update the "Velocity" member variable of UMovementComponent and apply
  /// all desired transformations to the updated component. There are a few things to keep in mind when implementing this:
  ///
  /// 1) This function will only ever be called by locally controlled pawns (client and listen server) and remotely controlled server pawns.
  /// Smoothed pawns can be updated manually with @see SimulatedTick if necessary.
  ///
  /// 2) If we are locally controlled, the out values of the passed move remain zeroed until after the move was executed, only the in values
  /// can be used meaningfully. If we are a remotely controlled server pawn however, the out variables of the passed move contain the
  /// replicated values received from the client.
  /// @attention This means that out values that have replication disabled are NAN (but only if @see bEnsureValidMoveData is set to false).
  ///
  /// 3) When running a listen server, the state of remotely controlled server pawns calculated within this function is a "virtual" state
  /// used to replicate back to the client for corrections. Although this is different from the actual visual representation of the pawn on
  /// the server (which is usually an interpolated state) you can still use functions like GetActorLocation() etc. as, for the time of the
  /// simulation, the pawn will actually be in that virtual state and will be set back to the smoothed state only after the simulation is
  /// complete.
  ///
  /// 4) For autonomous proxies: State values that are not replicated back from the server should never be changed within this function. For
  /// example, if replication of the actor rotation is disabled for the autonomous proxy server state, any changes made to the actor
  /// rotation in the replicated tick function would have no effect locally. You would have to modify the rotation from the outside if you
  /// want to change it locally and if you want these changes to still be reflected on the server, you should set the client rotation
  /// authoritatively (i.e. set @see bUseClientRotation to true). Keep in mind that this only works if the values in question are actually
  /// serialized within FMove.
  ///
  /// 5) In general, one should try to carry over as little data as possible between ticks. If it cannot be avoided, the variable that holds
  /// the persistent data must be bound to a pre-replicated member of the same type. Example: We have a bool "bIsGrounded" that tells us
  /// whether the pawn is on the ground or in the air. We could determine the value of "bIsGrounded" at the beginning of every tick from the
  /// pawn's location by sweeping downwards, in which case we would not have to replicate "bIsGrounded" because the value will not depend on
  /// any (non-replicated) information from the previous tick (as long as you do not do any operations that involve "bIsGrounded" before the
  /// sweep). If you are still unsure what data needs to be replicated a good strategy is to use local variables whenever possible. Usually
  /// the remaining data members that cannot be local variables have to be bound for replay. Keep in mind that binding a variable increases
  /// the download traffic of the client and upload traffic of the server. Per update and times the number of replicated pawns in the world,
  /// traffic is increased by at least 1 bit and at most by the size of the bound variable (disregarding possible padding). On average, most
  /// values that do not change very often take up considerably less bandwidth than those that change frequently.
  ///
  /// 6) For Blueprint: latent nodes (delays, timelines, etc.) or any nodes that save their state (flip-flop, do once, etc.) cannot be used.
  /// You will have to implement your own versions of them that bind the appropriate variables to pre-replicated data to carry over their
  /// state to the next tick.
  ///
  /// @param        Move                      The move containing the state of the pawn and all current input values for movement.
  /// @param        Iteration                 The iteration we are currently executing. Can be between 1 and @see MaxIterations (inclusive).
  /// @param        bIsSubSteppedIteration    Whether this is a sub-stepped iteration i.e. not the final one for the current move.
  /// @returns      void
  virtual void ReplicatedTick(const FMove& Move, int32 Iteration, bool bIsSubSteppedIteration) {}

  /// Tick function to update smoothed remotely controlled pawns manually on the local machine. The smooth state is usually an interpolated
  /// state between the start and target state. Location, actor rotation, control rotation and velocity are interpolated according to the
  /// configured interpolation method (@see InterpolationMethod). Bound data is not interpolated by default, the smooth state contains
  /// either the value of the  start or target state depending on which state we are currently closer to in time based on the set simulation
  /// delay (@see SimulationDelay). The passed start and target indices let you access the current start and target states from the state
  /// queue (@see AccessStateQueue). The simulated tick will also be called for remotely controlled server pawns when running a listen
  /// server but only if they are smoothed (@see bSmoothRemoteServerPawn). The simulated tick is intended for visual updates only
  /// (animations, effects, etc.) and should not be used for anything that can cause gameplay-critical side effects (use the
  /// @see OnSimulatedStateLoaded event for such logic instead). This function will never be called on a dedicated server.
  /// @attention If animations are tied to bound members they will usually play automatically without implementing any additional logic
  /// within the simulated tick (provided they are set to replicate).
  /// @attention The start and target state indices will be -1 if there are no valid states available this tick (e.g. when the state queue
  /// is empty), so don't use without checking.
  /// @attention You may not be able to catch every important change of a value by looking at just the start and target states because
  /// states can be skipped or reused by the interpolation algorithm. To illustrate this, assume we have a state queue with 5 entries that
  /// does not change for a few simulated ticks. For the first tick, the function passes you the index 1 for the start state and the index 2
  /// for the target state. It is possible that for the next tick the function passes you the index 4 for the start state and the index 5
  /// for the target state. If your logic only considers start and target state, you may miss important values that are saved within the
  /// state with the index 3 ("SkippedStateIndices" would contain the index 3 for this tick). Then for the next tick, the function may pass
  /// indices 4 and 5 again because we could still be interpolating between the same states. Although this issue is negligible in most cases
  /// it can be more prevalent if you have a machine that runs with a low framerate but receives network updates at a high frequency.
  /// @attention When the interpolation method is set to "None" the start and target indices will always be the same and will both refer to
  /// the most recently received state (the smooth state will also hold the same information).
  /// @attention The actual state the simulated pawn has assumed depends on the configuration. Usually most values are set from the smooth
  /// state but there are some exceptions, e.g. if @see bSmoothCollisionLocation is false the location of the pawn is set from the target
  /// state instead of the smooth state.
  ///
  /// @param        DeltaTime              The delta time for this frame (not sub-stepped).
  /// @param        SmoothState            The interpolated state between start and target state. Non-interpolated variables (bound members)
  ///                                      are initialized with the values closest to the smooth state's interpolation time. Can be an
  ///                                      invalid state in certain circumstances (timestamp will be -1). May also be an extrapolated state
  ///                                      if no recent enough data is available.
  /// @param        StartStateIndex        The index of the start state in the state queue. Can be -1 (meaning no state is available).
  /// @param        TargetStateIndex       The index of the target state in the state queue. Can be -1 (meaning no state is available).
  /// @param        SkippedStateIndices    All indices (if any) of states that have been skipped between the last target state and the
  ///                                      current start state (ordered newest to oldest).
  /// @returns      void
  virtual void SimulatedTick(
    float DeltaTime,
    const FState& SmoothState,
    int32 StartStateIndex,
    int32 TargetStateIndex,
    const TArray<int32>& SkippedStateIndices
  ) {}

  /// Overridable validation function for data received from the client. The return value is passed to the built-in RPC validation function
  /// and will cause a disconnect for the client when false is returned.
  /// @attention Timestamp verification will still run afterwards if enabled (@see bVerifyClientTimestamps) and is completely independent of
  /// the built-in validation (so remote moves may still get blocked from executing even if this function returned true).
  ///
  /// @param        RemoteMoves    The remote moves received from the client.
  /// @returns      bool           If true, the remote moves are considered valid. If false, the received moves are considered bad/invalid
  ///                              in the sense that they have possibly been tampered with (i.e. there may have been an attempt to cheat)
  ///                              and the client will be disconnected from the server.
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "Validate Remote Moves"))
  bool Server_ValidateRemoteMoves(const TArray<FMove>& RemoteMoves);
  virtual bool Server_ValidateRemoteMoves_Implementation(const TArray<FMove>& RemoteMoves) { return true; }

  /// Overridable function that gets called on the server every time the client exceeds the max strike count. Allows the user to handle the
  /// situation in an application specific way (e.g. disconnecting the client). The strike count is part of the timestamp verification
  /// process of client moves (has to be explicitly enabled, @see bVerifyClientTimestamps). Every time the server receives moves from the
  /// client, the timestamps are checked for plausibility. These checks are not completely accurate because of variable network latency, but
  /// if the client fails those checks frequently, the timestamps may be manipulated.
  ///
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "Handle Conspicuous Client"))
  void Server_HandleConspicuousClient();
  virtual void Server_HandleConspicuousClient_Implementation() {}

  /// Overridable function that gets called every time a client move is about to be executed on the server. Can be used if additional logic
  /// is required to put the server world into a state that is in sync with the client's world at the time the remote move was originally
  /// executed.
  ///
  /// @param        RemoteMove    The client move about to be executed on the server.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "Pre Remote Move Execution"))
  void Server_PreRemoteMoveExecution(const FMove& RemoteMove);
  virtual void Server_PreRemoteMoveExecution_Implementation(const FMove& RemoteMove) {}

  /// Overridable function that gets called after a client move was executed on the server.
  ///
  /// @param        RemoteMove    The client move that was just executed on the server.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "Post Remote Move Execution"))
  void Server_PostRemoteMoveExecution(const FMove& RemoteMove);
  virtual void Server_PostRemoteMoveExecution_Implementation(const FMove& RemoteMove) {}

  /// Overridable function that gets called before any client moves are executed on the server. Can be used to buffer the server world state
  /// if you plan to change it for remote move execution.
  /// @attention This is only called once per received client move batch as opposed to @see Server_PreRemoteMoveExecution which is called
  /// before every client move that is executed.
  ///
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "Pre Remote Moves Processing"))
  void Server_PreRemoteMovesProcessing();
  virtual void Server_PreRemoteMovesProcessing_Implementation() {}

  /// Overridable function that gets called after all client moves were executed and resolved on the server. Can be used to undo changes
  /// that were made to the server world for move execution.
  ///
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "On Remote Moves Processed"))
  void Server_OnRemoteMovesProcessed();
  virtual void Server_OnRemoteMovesProcessed_Implementation() {}

  /// Called when the client discrepancy is about to be resolved immediately after a remote move was executed and authoritative client
  /// values were set. The pawn state can still be adjusted at this point, but if it does not match the server state afterwards (within the
  /// configured tolerance) the move will be deemed invalid.
  /// @attention Values that the client did not sent to the server are replaced with the current server pawn state.
  ///
  /// @param        ClientLocation           The location value received from the client.
  /// @param        ClientRotation           The rotation value received from the client.
  /// @param        ClientControlRotation    The control rotation value received from the client.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "General Movement Component",
    meta = (DisplayName = "On Resolve Client Discrepancy"))
  void Server_OnResolveClientDiscrepancy(
    const FVector& ClientLocation,
    const FRotator& ClientRotation,
    const FRotator& ClientControlRotation
  );
  virtual void Server_OnResolveClientDiscrepancy_Implementation(
    const FVector& ClientLocation,
    const FRotator& ClientRotation,
    const FRotator& ClientControlRotation
  ) {}

  /// Overridable function that gets called every time a move is about to be executed during a client replay. Can be used if additional
  /// logic is required to put the client world into a state that is in sync with the world at the time the move was originally executed.
  ///
  /// @param        ReplayMove    The move about to be replayed.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component", meta = (DisplayName = "Pre Replay Move Execution"))
  void Client_PreReplayMoveExecution(const FMove& ReplayMove);
  virtual void Client_PreReplayMoveExecution_Implementation(const FMove& ReplayMove) {}

  /// Overridable function that gets called after a move was executed during a client replay.
  ///
  /// @param        ReplayMove    The move that was just replayed.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component", meta = (DisplayName = "Post Replay Move Execution"))
  void Client_PostReplayMoveExecution(const FMove& ReplayMove);
  virtual void Client_PostReplayMoveExecution_Implementation(const FMove& ReplayMove) {}

  /// Overridable function that gets called before any client moves are replayed. Can be used to buffer the client world state if you plan
  /// to change it for the replay.
  /// @attention This is only called once per replay as opposed to @see Client_PreReplayMoveExecution which is called before every move that
  /// is executed.
  ///
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component", meta = (DisplayName = "Pre Replay"))
  void Client_PreReplay();
  virtual void Client_PreReplay_Implementation() {}

  /// Overridable function that gets called at the end of a client replay after all moves were executed. Can be used to undo changes that
  /// were made to the client world for the replay.
  ///
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component", meta = (DisplayName = "On Moves Replayed"))
  void Client_OnMovesReplayed();
  virtual void Client_OnMovesReplayed_Implementation() {}

  /// Called after the pawn was set to a different state in an "immediate" context. If you have bound variables that have gameplay-critical
  /// side effects when their value changes you should put the logic for that here. Immediate contexts are mostly based on move queue data
  /// and are mainly used for move execution. They are usually simpler to handle than "simulated" contexts because the pawn just assumes one
  /// definitive state.
  ///
  /// @param        Context    The context in which the state was loaded.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  void OnImmediateStateLoaded(EImmediateContext Context);
  virtual void OnImmediateStateLoaded_Implementation(EImmediateContext Context) {}

  /// Called after the pawn was set to a different state in a "simulated" context. If you have bound variables that have gameplay-critical
  /// side effects when their value changes you should put the logic for that here. Simulated contexts are based on state queue data and are
  /// used for smoothing and rollback. They are usually more complex to handle than "immediate" contexts because the resulting state the
  /// pawn assumes can be a mix of different states.
  ///
  /// @param        SmoothState    The interpolated state.
  /// @param        StartState     The start state of the interpolation.
  /// @param        TargetState    The target state of the interpolation.
  /// @param        Context        The context in which the state was loaded.
  /// @returns      void
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  void OnSimulatedStateLoaded(
    const FState& SmoothState,
    const FState& StartState,
    const FState& TargetState,
    ESimulatedContext Context
  );
  virtual void OnSimulatedStateLoaded_Implementation(
    const FState& SmoothState,
    const FState& StartState,
    const FState& TargetState,
    ESimulatedContext Context
  ) {}

  /// Overridable function for custom checks to determine whether a move should be enqueued as a new move into the move queue (i.e. whether
  /// the move should not be combined with the previous one).
  /// @attention Only called when essential built-in checks have not already deemed this move to be important (e.g. a move will always be
  /// enqueued if combining would cause the max client delta time to be exceeded).
  ///
  /// @param        CurrentMove          The local move that was created this frame.
  /// @param        LastImportantMove    The last local move that was enqueued.
  /// @returns      int32                = 0: continue with the default checks to decide whether the current move should be enqueued.
  ///                                    > 0: enqueue the current move as a new move.
  ///                                    < 0: do not enqueue the current move as a new move (advanced).
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component", meta = (DisplayName = "Should Enqueue Move Custom"))
  int32 Client_ShouldEnqueueMove_Custom(const FMove& CurrentMove, const FMove& LastImportantMove) const;
  virtual int32 Client_ShouldEnqueueMove_Custom_Implementation(const FMove& CurrentMove, const FMove& LastImportantMove) const { return 0; }

  /// Custom interpolation function. Only affects velocity, location, actor rotation and control rotation. To modify other pawn data use the
  /// @see SimulatedTick function.
  ///
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states.
  /// @returns      FState                The resulting interpolated state.
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  FState InterpolateCustom1(const FState& StartState, const FState& TargetState, const float InterpolationRatio) const;
  virtual FState InterpolateCustom1_Implementation(const FState& StartState, const FState& TargetState, float InterpolationRatio) const;

  /// Custom interpolation function. Only affects velocity, location, actor rotation and control rotation. To modify other pawn data use the
  /// @see SimulatedTick function.
  ///
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states.
  /// @returns      FState                The resulting interpolated state.
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  FState InterpolateCustom2(const FState& StartState, const FState& TargetState, const float InterpolationRatio) const;
  virtual FState InterpolateCustom2_Implementation(const FState& StartState, const FState& TargetState, float InterpolationRatio) const;

  /// Custom interpolation function. Only affects velocity, location, actor rotation and control rotation. To modify other pawn data use the
  /// @see SimulatedTick function.
  ///
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states.
  /// @returns      FState                The resulting interpolated state.
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  FState InterpolateCustom3(const FState& StartState, const FState& TargetState, const float InterpolationRatio) const;
  virtual FState InterpolateCustom3_Implementation(const FState& StartState, const FState& TargetState, float InterpolationRatio) const;

  /// Custom interpolation function. Only affects velocity, location, actor rotation and control rotation. To modify other pawn data use the
  /// @see SimulatedTick function.
  ///
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states.
  /// @returns      FState                The resulting interpolated state.
  UFUNCTION(BlueprintNativeEvent, Category = "General Movement Component")
  FState InterpolateCustom4(const FState& StartState, const FState& TargetState, const float InterpolationRatio) const;
  virtual FState InterpolateCustom4_Implementation(const FState& StartState, const FState& TargetState, float InterpolationRatio) const;

  /// Tells us whether we are currently replaying moves from the move queue on a client. Useful for subclasses to branch on in their
  /// implementation of @see ReplicatedTick. Always returns false on the server and for simulated proxies.
  /// 告诉我们当前是否从客户端移动队列中重放移动。用于子类在@see ReplicatedTick的实现中进行分支。在服务器和模拟代理上总是返回false。
  ///
  /// @returns      bool    True if we are currently executing a client replay, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsReplaying() const;

  /// Tells us whether we are currently executing remote moves from a client on the server. Useful for subclasses to branch on in their
  /// implementation of @see ReplicatedTick. Always returns false on a client.
  ///
  /// @returns      bool    True if we are currently executing client moves on the server, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsExecutingRemoteMoves() const;

public:

  /// Get a reference to the owning pawn.
  ///
  /// @returns      AGenPawn*    Reference to the owning pawn.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  AGenPawn* GetGenPawnOwner() const;

  /// Tells us whether we are currently extrapolating the state of a smoothed remotely controlled pawn. Useful for subclasses to branch on
  /// in their implementation of @see SimulatedTick.
  ///
  /// @returns      bool    True if we are currently extrapolating the state of the pawn, false otherwise. Will always return false when
  ///                       extrapolation is disabled (@see bAllowExtrapolation) even when there is no recent enough state in the queue.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsExtrapolating() const;

  /// Gets the current server world time. When called on a client, this is the synced time with the network delay already accounted for.
  ///
  /// @returns      float    The time in seconds since the server world was brought up for play.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  float GetTime() const;

  /// Checks if we are the autonomous proxy on a client.
  /// 检查我们是否是客户端的自治代理。
  ///
  /// @returns      bool    True if our net role is ROLE_AutonomousProxy, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsAutonomousProxy() const;

  /// Checks if we are a simulated proxy on a client.
  ///
  /// @returns      bool    True if our net role is ROLE_SimulatedProxy, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsSimulatedProxy() const;

  /// Check if we are the autonomous or a simulated proxy on a client.
  ///
  /// @returns      bool    True if this pawn is an autonomous or simulated proxy, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsClientPawn() const;

  /// Check if we are a pawn on the server with authority.
  ///
  /// @returns      bool    True if this pawn has authority, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsServerPawn() const;

  /// Checks if we are a locally controlled pawn on a server.
  ///
  /// @returns      bool    True if we are a locally controlled pawn on a server, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsLocallyControlledServerPawn() const;

  /// Checks if we are a remotely controlled pawn on a server.
  ///
  /// @returns      bool    True if we are a pawn on a server that is controlled by a client, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsRemotelyControlledServerPawn() const;

  /// Checks if we are a locally controlled pawn on a dedicated server.
  ///
  /// @returns      bool    True if we are a locally controlled pawn on a dedicated server, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsLocallyControlledDedicatedServerPawn() const;

  /// Checks if we are a remotely controlled pawn on a dedicated server.
  ///
  /// @returns      bool    True if we are a pawn on a dedicated server that is controlled by a client, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsRemotelyControlledDedicatedServerPawn() const;

  /// Checks if we are a locally controlled pawn on a listen server.
  ///
  /// @returns      bool    True if we are a locally controlled pawn on a listen server, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsLocallyControlledListenServerPawn() const;

  /// Checks if we are a remotely controlled pawn on a listen server.
  ///
  /// @returns      bool    True if we are a pawn on a listen server that is controlled by a client, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsRemotelyControlledListenServerPawn() const;

  /// Checks if we are a remotely controlled pawn on a listen server and if the option to smooth those pawns is enabled. Whether the pawn
  /// is considered smoothed only depends on the value of @see bSmoothRemoteServerPawn, so this will return true even if the interpolation
  /// function is null (i.e. the interpolation method is "None").
  ///
  /// @returns      bool    True if we are a smoothed remotely controlled pawn on a listen server, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsSmoothedListenServerPawn() const;

  /// Check if we are a server pawn that is locally controlled by AI.
  ///
  /// @returns      bool    True if this pawn is a bot on the server, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsServerBot() const;

  /// Check if we are on a network server (i.e. not a client and not running in standalone).
  ///
  /// @returns      bool    True if we are a server in a networked context, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsNetworkedServer() const;

  /// Sets the function to be used for interpolation. The interpolation method can be changed at any time.
  ///
  /// @param        NewInterpolationMethod    The new interpolation method to use.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  void SetInterpolationMethod(EInterpolationMethod NewInterpolationMethod);

  /// Returns the latest interpolation data for remotely controlled pawns on the local machine. May also hold extrapolated data if no
  /// recent enough states are available to interpolate (regardless of the value of @see bAllowExtrapolation). The interpolated state
  /// usually reflects most of the actual current pawn state but some exceptions may apply (e.g. if @see bSmoothCollisionLocation or
  /// @see bSmoothCollisionRotation are false). There may not always be a valid interpolated state available (in which case the timestamp
  /// of the returned state will be -1), for example when the pawn was just spawned or if called on a locally controlled pawn.
  ///
  /// @returns      const FState&    Reference-to-const to the current interpolation state for remotely controlled pawns.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  const FState& GetLastInterpolationState() const;

  /// Returns the start state that was used during the last interpolation of a remotely controlled pawn (@see GetLastInterpolationState).
  /// There may not always be a valid start state available (in which case the timestamp of the returned state will be -1), for example
  /// when the pawn was just spawned or if called on a locally controlled pawn.
  ///
  /// @returns      const FState&    Reference-to-const to the current interpolation start state of a remotely controlled pawn.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  const FState& GetLastInterpolationStartState() const;

  /// Returns the target state that was used during the last interpolation of a remotely controlled pawn (@see GetLastInterpolationState).
  /// There may not always be a valid target state available (in which case the timestamp of the returned state will be -1), for example
  /// when the pawn was just spawned or if called on a locally controlled pawn.
  ///
  /// @returns      const FState&    Reference-to-const to the current interpolation target state of a remotely controlled pawn.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  const FState& GetLastInterpolationTargetState() const;

  /// Returns the index of the current interpolation start state in the state queue. The index is guaranteed to be updated per tick while
  /// the actual start state as returned by @see GetLastInterpolationStartState is not, so this may return -1 if no states to interpolate
  /// are available this tick.
  ///
  /// @returns      int32    The index into the state queue for the interpolation start state of the current tick if available. Always
  ///                        returns -1 for non-smoothed pawns.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  int32 GetCurrentInterpolationStartStateIndex() const;

  /// Returns the index of the current interpolation target state in the state queue. The index is guaranteed to be updated per tick while
  /// the actual target state as returned by @see GetLastInterpolationTargetState is not, so this may return -1 if no states to interpolate
  /// are available this tick.
  ///
  /// @returns      int32    The index into the state queue for the interpolation target state of the current tick if available. Always
  ///                        returns -1 for non-smoothed pawns.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  int32 GetCurrentInterpolationTargetStateIndex() const;

  /// Whether a remotely controlled pawn was updated through smoothing during the current tick. Returns false if the state queue did not
  /// have enough states available to interpolate or if the function is called on a non-smoothed pawn. The interpolation states
  /// (@see GetLastInterpolationState, GetLastInterpolationStartState, GetLastInterpolationTargetState) always hold the data from the last
  /// interpolation even if the pawn was not updated within the current tick so this function allows you to check whether that information
  /// is up-to-date.
  /// @attention Even if the interpolation method is set to "None" the pawn is still considered smoothed.
  ///
  /// @returns      bool    Whether the current interpolation data for a smoothed pawn is current.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsInterpolationDataCurrent() const;

  /// Access the state in the default state queue at the given (valid) index. Read only.
  ///
  /// @param        Index      The index at which to access the state queue. Must be a valid index so check beforehand.
  /// @returns      FState&    Reference-to-const to the state at the specified index.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  const FState& AccessStateQueue(int32 Index) const;

  /// Checks if the passed index corresponds to a valid state within the state queue.
  ///
  /// @param        Index    The index to check.
  /// @returns      bool     True if the passed index maps to a valid state within the state queue, false otherwise.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  bool IsValidStateQueueIndex(int32 Index) const;

  /// Returns the current number of entries in the state queue.
  ///
  /// @returns      int32    The size of the state queue.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  int32 GetStateQueueSize() const;

  /// Returns the max number of entries the state queue can hold.
  ///
  /// @returns      int32    The max size of the state queue.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  int32 GetStateQueueMaxSize() const;

  /// Returns the replicated and possibly interpolated control rotation in the local space of the pawn's root component. Also returns a
  /// valid control rotation if called on a simulated proxy (which have no controller). If a control rotation component is not being
  /// replicated, it will reflect the current local value.
  ///
  /// @returns      FRotator    The current control rotation in local space.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FRotator GetControlRotationLocalSpace() const;

  /// Returns the current velocity of the updated component.
  ///
  /// @returns      FVector    Velocity of the updated component (member of UMovementComponent).
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual FVector GetVelocity() const;

  /// Sets the velocity member of UMovementComponent to the new velocity and calls UpdateComponentVelocity() afterwards.
  ///
  /// @param        NewVelocity    The new velocity vector.
  /// @returns      void
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  virtual void SetVelocity(const FVector& NewVelocity);

private:

  UPROPERTY(Transient, DuplicateTransient)
  /// The component's owning pawn.
  AGenPawn* GenPawnOwner{nullptr};

#pragma region Move Execution

  /// Updates the local move every tick with the current input data.
  ///
  /// @returns      void
  void RefreshLocalMove();

  /// Executes the passed move.
  ///
  /// @param        Move       The move to execute.
  /// @param        Context    The context in which the move is executed.
  /// @returns      FVector    The location delta i.e. the distance the pawn has moved from the input location.
  FVector ExecuteMove(const FMove& Move, EImmediateContext Context);

  /// Calculates the (sub-stepped) delta time for the current iteration of a move execution.
  ///
  /// @param        Iterations       The current iteration number.
  /// @param        RemainingTime    The remaining time of the original delta time.
  /// @returns      float            The sub-stepping delta time for this iteration.
  float CalculateSubDeltaTime(int32 Iterations, float RemainingTime) const;

protected:

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (ClampMin = "0.000001", UIMin = "0.01", EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// The time interval in which to subdivide the delta time of a move for move execution (if the delta time actually exceeds this value).
  /// The remaining time will always be evenly divided for the last two iterations of a move execution.
  float MaxTimeStep{0.033334f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (ClampMin = "1", UIMin = "1", EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// How many iterations of a sub-stepped move execution are allowed at most. If upon reaching the last iteration there is still more time
  /// than the max allowed timestep remaining, all the remaining time will be used in the last iteration.
  int32 MaxIterations{10};

#pragma endregion

#pragma region Movement Replication

  UPROPERTY(BlueprintReadOnly, Category = "General Movement Component", DisplayName = "SimulatedRoot")
  /// This component is introduced into a pawn's scene component hierarchy just underneath the root component. It is used to move the other
  /// attached components independently of the root collision.
  class USceneComponent* SimulatedRootComponent{nullptr};

private:

  /// Do not bind any pre-replicated data members on objects that have any of these flags set (@see BindReplicationData).
  /// @attention More flags may need to be excluded in future versions.
  EObjectFlags NoBindingFlags = RF_ClassDefaultObject | RF_ArchetypeObject | RF_NeedLoad | RF_WillBeLoaded;

  /// Shorter version of the static method that fills NaN components with values from the current velocity, e.g. GetValidVelocity(V) returns
  /// the same vector as GetValidVector(V, GetVelocity()).
  ///
  /// @param        UnpackedVelocity    The velocity vector that was unpacked by the receiver which may contain NaN values.
  /// @returns      FVector             The valid velocity which may contain components replaced with values of the current velocity.
  FVector GetValidVelocity(const FVector& UnpackedVelocity) const;

  /// Shorter version of the static method that fills NaN components with values from the current actor location.
  ///
  /// @param        UnpackedLocation    The location vector that was unpacked by the receiver which may contain NaN values.
  /// @returns      FVector             The valid location which may contain components replaced with values of the current actor location.
  FVector GetValidActorLocation(const FVector& UnpackedLocation) const;

  /// Shorter version of the static method that fills NaN components with values from the current actor rotation.
  ///
  /// @param        UnpackedRotation    The rotation that was unpacked by the receiver which may contain NaN values.
  /// @returns      FRotator            The valid rotation which may contain components replaced with values of the current actor rotation.
  FRotator GetValidActorRotation(const FRotator& UnpackedRotation) const;

  /// Shorter version of the static method that fills NaN components with values from the current control rotation.
  ///
  /// @param        UnpackedControlRotation    The control rotation that was unpacked by the receiver which may contain NaN values.
  /// @returns      FRotator                   The valid control rotation which may contain components replaced with values of the current
  ///                                          control rotation.
  FRotator GetValidControlRotation(const FRotator& UnpackedControlRotation) const;

  /// Utility function to fill the passed move with data.
  ///
  /// @param        Move          The move to fill.
  /// @param        VarsToFill    The variables (input or output) to fill.
  /// @returns      void
  void FillMoveWithData(FMove& Move, FMove::EStateVars VarsToFill) const;

  /// Sets the pawn state and updates the values of bound member variables from the input data of the passed move before move execution.
  ///
  /// @param        MoveToLoad    The move to load the input data from.
  /// @param        Context       The context in which the move is being loaded.
  /// @returns      void
  void LoadMove(const FMove& MoveToLoad, EImmediateContext Context);

protected:

  /// Saves the local state of the pawn into the passed state.
  ///
  /// @param        OutState    Holds the local state of the pawn at the time when the function was called.
  /// @returns      void
  virtual void SaveLocalPawnState(FState& OutState) const;

  /// Sets the local pawn state from the data contained in the passed state. Calls @see OnImmediateStateLoaded after the pawn state was set.
  ///
  /// @param        State      The state to load from.
  /// @param        Context    The context in which the state is being set.
  /// @returns      void
  virtual void SetLocalPawnState(const FState& State, EImmediateContext Context);

  /// Sets the pawn state from the data contained in the passed states, but only sets bound values that are set to be net serialized within
  /// the passed "SmoothState". Calls @see OnSimulatedStateLoaded after the pawn state was set. Considers the values of
  /// @see bSmoothCollisionLocation and @see bSmoothCollisionRotation.
  ///
  /// @param        SmoothState    The interpolated state. Determines which bound values will be set.
  /// @param        StartState     The start state of the interpolation.
  /// @param        TargetState    The target state of the interpolation.
  /// @param        Context        The context in which the state is being set.
  /// @returns      void
  void SetReplicatedPawnState(const FState& SmoothState, const FState& StartState, const FState& TargetState, ESimulatedContext Context);

  /// Retrieves the server state for either the autonomous or simulated proxy based on the passed net role. Other enum values are not valid
  /// and will fail a check macro.
  ///
  /// @param        RecipientRole    The net role of the recipient of the server state (ROLE_AutonomousProxy or ROLE_SimulatedProxy).
  /// @returns      FState&          A reference to the autonomous or simulated proxy server state.
  FState& GetServerStateFromRole(ENetRole RecipientRole);
  const FState& GetServerStateFromRole(ENetRole RecipientRole) const;

  /// Retrieves the buffered server state for either the autonomous or simulated proxy based on the passed net role. Other enum values are
  /// not valid and will fail a check macro.
  ///
  /// @param        RecipientRole    The net role of the recipient of the server state (ROLE_AutonomousProxy or ROLE_SimulatedProxy).
  /// @returns      FState&          A reference to the previous autonomous or simulated proxy server state.
  FState& GetLastServerStateFromRole(ENetRole RecipientRole);
  const FState& GetLastServerStateFromRole(ENetRole RecipientRole) const;

  /// Saves the input mode to the passed move.
  ///
  /// @param        Move          The move to save the input mode to.
  /// @param        VarsToSave    Whether to save to the input or output members of the move.
  /// @returns      void
  virtual void SaveInputModeToMove(FMove& Move, FMove::EStateVars VarsToSave) const;

  /// Saves the input mode to the passed state.
  ///
  /// @param        State    The state to save the input mode to.
  /// @returns      void
  virtual void SaveInputModeToState(FState& State) const;

  /// Loads the input mode from the passed move.
  ///
  /// @param        Move    The move to load the input mode from.
  /// @returns      void
  virtual void LoadInInputModeFromMove(const FMove& Move);

  /// Loads the input mode from the passed state.
  ///
  /// @param        State    The state to load the input mode from.
  /// @returns      void
  virtual void LoadInputModeFromState(const FState& State);

  /// Loads the input mode from the passed state if it is replicated to simulated proxies.
  ///
  /// @param        State    The state to load the input mode from.
  /// @returns      void
  virtual void LoadReplicatedInputModeFromState(const FState& State);

  /// Returns the passed input mode enum value as a string. Mainly used for debugging purposes.
  ///
  /// @param        InputMode    The input mode enum value.
  /// @returns      FString      The passed enum value as string.
  virtual FString GetInputModeAsString(EInputMode InputMode) const;

private:

  /// The last client move that was unpacked on the server.
  FMove Server_LastUnpackedClientMove;

  /// Whether the last client move that was processed on the server was valid or not.
  bool Server_bLastClientMoveWasValid{false};

  /// Flag to ensure that we initialize the state buffer before we swap it for the first time.
  bool Server_bStateBufferInitialized{false};

  /// Timer handle to set the flags for forcing full serialization of replication data (@see Server_SetForceFullSerializationFlagPeriodic).
  FTimerHandle Server_SetForceFullSerializationFlagPeriodicHandle;

  /// Timer handle to reset the client strikes when a verification interval is complete (@see Server_ResetClientStrikes).
  FTimerHandle Server_ResetClientStrikesHandle;

  /// The current amount of strikes the client has received for falsely timestamped data within the current verification interval.
  int32 Server_ClientStrikeCount{0};

  /// Whether we are currently executing client moves on the server. Can be queried with @see IsExecutingRemoteMoves by subclasses.
  bool Server_bIsExecutingRemoteMoves{false};

  /// Server RPC called by the autonomous proxy to send the pending client moves to the server.
  ///
  /// @param        RemoteMoves    The array with the autonomous proxy moves to send to the server.
  /// @returns      void
  UFUNCTION(Server, Reliable, WithValidation)
  void Server_SendMoves(const TArray<FMove>& RemoteMoves);
  void Server_SendMoves_Implementation(const TArray<FMove>& RemoteMoves);
  bool Server_SendMoves_Validate(const TArray<FMove>& RemoteMoves);

  /// Resets strikes the client has accumulated due to invalid timestamps. This is a timed function (@see Server_ResetClientStrikesHandle)
  /// and called every @see StrikeResetInterval seconds when timestamp verification is enabled.
  ///
  /// @returns      void
  void Server_ResetClientStrikes();

  /// Evaluates a move received from the client. Values that have not changed since the last update are copied from the values received with
  /// the last move.
  ///
  /// @param        Move     The received client move to unpack.
  /// @returns      FMove    The evaluated/unpacked client move, filled with the appropriate values.
  FMove Server_UnpackClientMove(const FMove& Move);

  /// Overwrites NAN values in the passed move (i.e. replication data that was not received from the client) with 0.
  ///
  /// @param        Move    The move for which to ensure valid data.
  /// @returns      void
  void Server_EnsureValidMoveData(FMove& Move);

  /// Tries to resolve differences between the client and server. If the deviations between the values received from the client and the
  /// values calculated on the server are smaller than the set tolerance, the server adopts the client state and marks the last client move
  /// as valid.
  /// @see MaxVelocityError
  /// @see MaxLocationError
  /// @see MaxRotationError
  /// @see MaxControlRotationError
  ///
  /// @param        ServerLocation           The server calculated location.
  /// @param        ServerRotation           The server calculated rotation.
  /// @param        ServerControlRotation    The server calculated control rotation.
  /// @param        ClientLocation           The out location of the client move.
  /// @param        ClientRotation           The out rotation of the client move.
  /// @param        ClientControlRotation    The out control rotation of the client move.
  /// @param        MoveTimestamp            The timestamp of the move that was executed. Used for debug logging.
  /// @returns      bool                     True if the discrepancies could be resolved (client move was valid), false otherwise.
  bool Server_ResolveClientDiscrepancy(
    const FVector& ServerLocation,
    const FRotator& ServerRotation,
    const FRotator& ServerControlRotation,
    const FVector& ClientLocation,
    const FRotator& ClientRotation,
    const FRotator& ClientControlRotation,
    float MoveTimestamp
  );

  /// Quantize the state of a remotely controlled server pawn from the values saved in the passed state. This means that the values inside
  /// the state will be quantized and that the pawn will be set to that quantized state.
  /// @attention The quantization level is determined by the settings of the passed state.
  ///
  /// @param        ServerState    The server state to quantize and to set the pawn state from.
  /// @returns      void
  void Server_QuantizePawnStateFrom(FState& ServerState);

  /// Determines whether a net update should be forced because a value has changed that the client needs to know about. For most variables
  /// (e.g. location) it is not necessary (or practical) that the client receives an update every time the value changes because we can
  /// interpolate between them. For some variables (usually those of a more discrete type e.g. booleans) this is desired however so a net
  /// update can be forced when the value changes.
  /// @attention Although server to client replication is reliable, it does happen sometimes that a server state is not replicated to the
  /// client even though a net update was forced, usually because the value changed only for a very short period of time. A value might also
  /// be skipped by the interpolation algorithm, even though it was replicated. Either way this is not a critical problem, but could cause
  /// missing animations for simulated proxies sometimes if they are tied to a replicated variable. To minimize the chances of this
  /// happening, you can increase the value of @see MinRepHoldTime.
  ///
  /// @param        RecipientRole    The net role of the recipient of the server state (ROLE_AutonomousProxy or ROLE_SimulatedProxy).
  /// @returns      bool             True if a net update should be forced because some important value has changed, false otherwise.
  bool Server_ShouldForceNetUpdate(ENetRole RecipientRole);

  /// Custom checks to force a replication update to the client. Use @see GetServerStateFromRole and @see GetLastServerStateFromRole to
  /// access the appropriate server states for the current recipient. Only called if the default checks have not already decided to force
  /// a net update.
  ///
  /// @param        RecipientRole    The net role of the recipient of the server state (ROLE_AutonomousProxy or ROLE_SimulatedProxy).
  /// @returns      bool             True if a net update should be forced because some important value has changed, false otherwise.
  virtual bool Server_ShouldForceNetUpdate_Custom(ENetRole RecipientRole) const { return false; }

  /// Save the current server state for replication to the clients.
  ///
  /// @param        RecipientRole    The net role of the recipient of the server state (ROLE_AutonomousProxy or ROLE_SimulatedProxy).
  /// @param        SourceMove       The move previously executed i.e. the move that led to the current pawn state.
  /// @returns      void
  void Server_SaveServerState(ENetRole RecipientRole, const FMove& SourceMove);

  /// Utility function to fill the passed server state with data.
  ///
  /// @param        State            The server state to fill.
  /// @param        RecipientRole    The recipient of the server state (ROLE_AutonomousProxy or ROLE_SimulatedProxy).
  /// @returns      void
  void Server_FillServerStateWithData(FState& State, ENetRole RecipientRole) const;

  /// Initializes the state buffer with the current pawn state before we swap the state for the first time.
  ///
  /// @returns      void
  void Server_InitializeStateBuffer();

  /// Called when the state buffer is initialized.
  ///
  /// @param        StateBuffer    The initialized state buffer.
  /// @returns      void
  virtual void Server_OnInitializeStateBuffer(const FState& StateBuffer) {}

  /// Swaps a listen server pawn's state from smoothed to un-smoothed and vice versa. Used during processing of received client moves on the
  /// server. Like simulated proxies, remotely controlled listen server pawns are usually smoothed through interpolation. If we don't do
  /// this the movement visuals for those pawns will most likely be very choppy. However, the interpolated values won't be the same as the
  /// client's actual movement so we need to differentiate between the smoothed server state and the state that is used in the execution of
  /// the client move (of which the result is also replicated back to the autonomous proxy). Therefore we need to un-smooth the pawn before
  /// simulating the client moves and re-smooth it again afterwards.
  ///
  /// @param        Context    The context in which the state buffer is swapped.
  /// @returns      void
  void Server_SwapStateBuffer(EImmediateContext Context);

  /// Called when the state buffer is swapped.
  ///
  /// @param        StateBuffer    The swapped state buffer.
  /// @param        Context        The context in which the state buffer is being swapped ("UnSmoothingRemoteListenServerPawn" or
  ///                              "ReSmoothingRemoteListenServerPawn").
  /// @returns      void
  virtual void Server_OnSwapStateBuffer(const FState& StateBuffer, EImmediateContext Context) {}

  /// Gets an FState for a server pawn to add to its state queue. This is a revised version of the simulated proxy server state that is
  /// filtered for values that the client may not be replicating to the server, having them replaced with NaNs (the server state takes its
  /// values directly from the pawn state, so NaN values from the unpacked client move are not preserved). This is essentially a local
  /// server version of what happens when the client unpacks a replication update from the server.
  ///
  /// @returns      FState    The filtered server state, possibly containing NaN values for data not received from the client.
  FState Server_GetFilteredServerState() const;

  /// Sets the flag to force full serialization of replication data when an actor becomes net relevant. When this happens, all data has to
  /// be fully serialized again because the actor may have been destroyed on the client (which means the information about which values were
  /// received previously was lost as well).
  ///
  /// @returns      void
  void Server_CheckNetRelevancy();

  /// Sets the flag to force full serialization of replication data when the interval time determined by @see FullSerializationInterval was
  /// reached. Used to mitigate the (rare) problem of client pawns getting stuck in the wrong state when the server assumes a packet was
  /// received when in fact it got lost. This is a timed function (@see Server_SetForceFullSerializationFlagPeriodicHandle).
  ///
  /// @returns      void
  void Server_SetForceFullSerializationFlagPeriodic();

protected:

  /// Generic function for processing the received client moves on the server.
  /// @attention This function is intentionally not private to be able to implement custom serialization settings for client moves, but it
  /// should otherwise not be used by child classes.
  ///
  /// @param        RemoteMoves    The array with the autonomous proxy moves received from the client.
  /// @returns      void
  void Server_ProcessClientMoves(const TArray<FMove>& RemoteMoves);

  /// Verifies the timestamps of the moves received from the client. Can be overridden to implement additional or different checks. A failed
  /// verification will increase the client strike count and may block client moves from being executed, but unlike the validation function
  /// (@see Server_ValidateRemoteMoves) it will never cause the client to disconnect.
  ///
  /// @param        RemoteMoves    The array with the moves received from the client.
  /// @returns      bool           True if all the timestamps were valid, false otherwise.
  virtual bool Server_VerifyTimestamps(const TArray<FMove>& RemoteMoves) const;

  /// Updates the map of currently connected players and their last serialized data (@see FState::LastSerialized). The list is actively
  /// maintained and verified with every replication update which is a bit inefficient but we want to minimize dependencies. Can be
  /// overridden to implement a more optimized version (e.g. using the game mode class).
  ///
  /// @returns      void
  virtual void Server_MaintainSerializationMap();

public:

  /// Called from @see FState::NetSerialize when the current server state is replicated to a simulated proxy in order to set the appropriate
  /// flag in the latest state queue entry.
  ///
  /// @param        TargetConnection    The connection that the server state will be replicated to.
  /// @returns      void
  void Server_SetReplicationFlag(APlayerController* TargetConnection);

private:

  /// Autonomous proxy move queue. Moves created by the autonomous proxy are appended to the queue (lower index means older move).
  TArray<FMove> Client_MoveQueue;

  /// Contains all completed moves that are waiting to be send to the server with the next RPC call.
  TArray<FMove> Client_PendingMoves;

  /// The previously received server state (one update earlier than the current one). Buffered for comparison with the values of the newly
  /// received state. If values have not changed the value from the last received server state is used again.
  FState Client_LastReceivedServerState;

  /// The last move that was added to the move queue. Used for comparison to determine whether the new move can be combined with the
  /// previous one (if no important values have changed). This will never be a combined move meaning any values saved will be the values of
  /// the move when it was originally enqueued, even if the enqueued move got combined with others later on.
  FMove Client_LastSignificantMove;

  /// The time in seconds since the last batch of moves was sent from the client to the server. We keep track of this to be able to meet the
  /// client send rate.
  float Client_TimeSinceLastMoveWasSent{false};

  /// Whether we are currently performing a client replay. Only true during the actual replay loop and always false on the server. Can be
  /// queried with @see IsReplaying by subclasses.
  bool Client_bIsReplaying{false};

  /// The last values that were marked to be serialized on the client and sent to the server. We refer to these when the next batch is to be
  /// sent to determine whether values have changed and need to be serialized again.
  float Client_LastSentInputVectorX{0.f};
  float Client_LastSentInputVectorY{0.f};
  float Client_LastSentInputVectorZ{0.f};
  float Client_LastSentOutRotationRoll{0.f};
  float Client_LastSentOutRotationPitch{0.f};
  float Client_LastSentOutRotationYaw{0.f};
  float Client_LastSentControlRotationRoll{0.f};
  float Client_LastSentControlRotationPitch{0.f};
  float Client_LastSentControlRotationYaw{0.f};
  FVector Client_LastSentOutVelocity{0};
  FVector Client_LastSentOutLocation{0};

  /// Wrapper function around the autonomous proxy's move execution that does some additional client specific logic.
  ///
  /// @param        Move           In: the move to execute containing the input. Out: the executed move filled with the output.
  /// @param        Context        The context in which the client move is being executed.
  /// @param        bReplicated    Whether this move will be replicated to the server or not.
  /// @returns      void
  void Client_ExecuteMove(FMove& Move, EImmediateContext Context, bool bReplicated = true);

  /// Deletes moves already acknowledged by the server from the move queue. Only moves that are newer than the timestamp we have received
  /// from the server are kept, moves that are older were already executed on the server. The source move is the move that has the same
  /// timestamp as the received state. This move was the original input for the state that we received and is used to check the deviation
  /// of the client state from the server state.
  /// @attention The returned source move will contain NANs for state values that are not replicated by the server.
  ///
  /// @param        ReceivedTimestamp    The timestamp of the last replicated server state.
  /// @returns      FMove                The source move of the received server state (possibly containing NANs).
  FMove Client_ClearAcknowledgedMoves(float ReceivedTimestamp);

  /// Determines whether the client should replay, mainly based on the comparison of the replicated server state values with the values from
  /// the source move. If they differ by more than the allowed max deviations the client should replay. If the source move was valid, only
  /// the velocity is checked.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state (i.e. the move with the same
  ///                             timestamp as the received server state).
  /// @returns      bool          True if the client should replay, false if not.
  bool Client_ShouldReplay(const FMove& SourceMove) const;

  /// Executes a client replay. Sets the client pawn to the server state and replays all moves in the move queue.
  ///
  /// @returns      void
  void Client_ReplayMoves();

  /// Set the client state to the replicated server values for replay. If the source move was valid, only the velocity and bound data will
  /// be set from the server state, because the other properties were determined to be correct on the server and not replicated (so we can
  /// take those values from the source move). If the source move was not valid (i.e. the server state contains all of the replicated data),
  /// all properties will be adopted from the server state.
  ///
  /// @param        bContainsFullRepBatch    Whether the server state contains the full set of replicated data. If it does, this implies
  ///                                        that the source move was not valid and the server wants us to replay.
  /// @param        SourceMove               The source move with the same timestamp as the received server state.
  /// @returns      void
  void Client_AdoptServerState(bool bContainsFullRepBatch, const FMove& SourceMove);

  /// Checks if the current velocity of the client is valid. This is done by comparing the value received from the server with the output of
  /// the move that was the source of the received server state (i.e. the move with the same timestamp as the received state). If the values
  /// are equal (deviation smaller than the max allowed error) the velocity is valid.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state.
  /// @returns      bool          True if the client velocity is valid, false otherwise.
  bool Client_IsVelocityValid(const FMove& SourceMove) const;

  /// Checks if the current location of the client is valid. This is done by comparing the value received from the server with the output of
  /// the move that was the source of the received server state (i.e. the move with the same timestamp as the received state). If the values
  /// are equal (deviation smaller than the max allowed error) the location is valid. Only checked when the source move was not valid.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state.
  /// @returns      bool          True if the client location is valid, false otherwise.
  bool Client_IsLocationValid(const FMove& SourceMove) const;

  /// Checks if the current rotation of the client is valid. This is done by comparing the value received from the server with the output of
  /// the move that was the source of the received server state (i.e. the move with the same timestamp as the received state). If the values
  /// are equal (deviation smaller than the max allowed error) the rotation is valid. Only checked when the source move was not valid.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state.
  /// @returns      bool          True if the client rotation is valid, false otherwise.
  bool Client_IsRotationValid(const FMove& SourceMove) const;

  /// Checks if the current control rotation of the client is valid. This is done by comparing the value received from the server with the
  /// output of the move that was the source of the received server state (i.e. the move with the same timestamp as the received state). If
  /// the values are equal (deviation smaller than the max allowed error) the control rotation is valid.  Only checked when the source move
  /// was not valid.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state.
  /// @returns      bool          True if the client control rotation is valid, false otherwise.
  bool Client_IsControlRotationValid(const FMove& SourceMove) const;

  /// Checks if the current input mode of the client is valid. This is done by comparing the value received from the server with the output
  /// of the move that was the source of the received server state (i.e. the move with the same timestamp as the received state). If the
  /// values are equal the input mode is valid.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state.
  /// @returns      bool          True if the client input mode is valid, false otherwise.
  bool Client_IsInputModeValid(const FMove& SourceMove) const;

  /// Sets the input mode of the pawn for replay. If the input mode was replicated it will be loaded from the autonomous proxy server state,
  /// otherwise it will be loaded from the source move.
  ///
  /// @param        SourceMove    The move that contained the source data for the received server state.
  /// @returns      void
  void Client_LoadInputModeForReplay(const FMove& SourceMove);

  /// Manages the move queue by either adding the new move to the queue, combining it with the previous one, or discarding it because it has
  /// an invalid timestamp (which can happen after the local time was synchronised with the server time) or because the move queue is full.
  ///
  /// @param        NewMove               The current local move which will be added, combined or discarded.
  /// @param        bOutStartedNewMove    True: the new move was added to the queue as a new entry. False: the new move was combined with
  ///                                     the last move in the queue.
  /// @param        bOutMoveQueueFull     True if the move queue is full, false otherwise.
  /// @returns      bool                  True if the new move was processed successfully (either added or combined), false if it was
  ///                                     discarded (either because of an invalid timestamp or because the move queue was full).
  bool Client_MaintainMoveQueue(const FMove& NewMove, bool& bOutStartedNewMove, bool& bOutMoveQueueFull);

  /// Checks if the current move should be added to the move queue. We only enqueue a move if important values have changed since the last
  /// move was added (save for some special conditions), otherwise moves are combined.
  ///
  /// @param        CurrentMove    The local move that was created this frame.
  /// @returns      bool           True if the current move should be enqueued as a new move, false otherwise.
  bool Client_ShouldEnqueueMove(const FMove& CurrentMove);

  /// Determines whether it is time to send the pending client moves to the server based on the set client send rate.
  ///
  /// @returns      bool    True if the pending moves should be sent this frame, false otherwise.
  bool Client_ShouldSendMoves();

  /// Adds a move to the move queue if there is still space for it.
  ///
  /// @param        NewMove              The move to add to the move queue.
  /// @param        bOutMoveQueueFull    True if the move queue is full (maybe after enqueueing the new move), false otherwise.
  /// @returns      bool                 True if the new move was added to the queue, false otherwise.
  bool Client_AddToMoveQueue(const FMove& NewMove, bool& bOutMoveQueueFull);

  /// Evaluates a replication update received from the server. Values that were not newly read are copied from the last deserialized values,
  /// which for the autonomous proxy are not necessarily those received with the last server state update (the autonomous proxy only
  /// receives the location, rotation and control rotation if a move was determined to be invalid by the server). The actor velocity and any
  /// other replicated data is always deserialized if replicated even for the autonomous proxy (in the sense that at least one bit is
  /// sent/received per property).
  ///
  /// @param        ServerState    The replicated server state to unpack.
  /// @returns      void
  void Client_UnpackReplicationUpdate(FState& ServerState);

  /// Quantizes (i.e. rounds values with regard to their compression level) the client state after the movement was performed from the out
  /// values of the passed move. This keeps the local state aligned with the data the server actually receives.
  /// @attention The quantization level is determined by the settings of the passed move except for the velocity vector, which is quantized
  /// according to the setting in the autonomous proxy server state.
  ///
  /// @param        Move    The executed move that holds the client data that should be used for quantization.
  /// @returns      void
  void Client_QuantizePawnStateFrom(FMove& Move);

  /// Evaluates which values have changed compared to the last sent move and therefore need to be serialized again. We send one additional
  /// bit that indicates whether a value has changed or not. That way a lot of bandwidth is saved for values that don't change very often.
  ///
  /// @returns      void
  void Client_DetermineValuesToSend();

protected:

  /// Called when a server state update is received for the autonomous proxy. Unpacks the replicated data, evaluates the client state, and
  /// corrects it by executing a replay when necessary.
  ///
  /// @returns      void
  UFUNCTION()
  virtual void Client_OnRepServerState_AutonomousProxy();

  /// Called when a server state update is received for a simulated proxy. Unpacks the replicated state and adds it to the state queue for
  /// smoothing.
  ///
  /// @returns      void
  UFUNCTION()
  virtual void Client_OnRepServerState_SimulatedProxy();

  /// Called on the client when adopting the server state before a replay.
  /// @attention Bound data saved within the source move and server state can be referenced directly (C++ only) with the name of the type
  /// (as defined in the pre-replicated data header) and an integer designating the n-th binding of that particular type. For example, to
  /// reference the second bound boolean from the server state you would write ServerState.Bool2, to reference the same data from the source
  /// move you would write SourceMove.OutBool2 (as the out-values of the move are corresponding to the server state values). This also means
  /// that if you change the order in which variables are bound, the references to those variables have to be changed as well.
  ///
  /// @param        SourceMove     The source move with the same timestamp as the received server state.
  /// @param        ServerState    The received autonomous proxy server state.
  /// @returns      void
  virtual void Client_OnServerStateAdopted(const FMove& SourceMove, const FState& ServerState) {}

  /// Determines whether the client is allowed to perform a replay from an invalid source move. By default the only condition that is
  /// checked here is whether we are exceeding the velocity threshold when @see bOnlyReplayWhenMoving is enabled but custom checks can be
  /// implemented in derived classes.
  /// @attention Only affects replays that are triggered from server-verified data. If a client-side check triggers a replay because the
  /// input mode, bound data or the velocity deviates the replay is always executed.
  ///
  /// @returns      bool    True if the client is allowed to perform the replay, false otherwise.
  virtual bool Client_IsAllowedToReplay() const;

  /// Buffers non-replicated/local-only values of the client state so they can be isolated from the net code. This is only applies to the
  /// pawn's velocity, location, actor rotation, control rotation and input mode by default. If a value is not replicated (from server to
  /// autonomous proxy that is), the replicated tick should not change that value at all, but it can then be modified freely from the
  /// outside. Any changes made inside the replicated tick function will have no effect locally anymore. This is most often used for the
  /// client's control rotation, which is usually modified directly outside the movement component and not replicated back to the client.
  /// @see Client_ApplyBufferedLocalState
  ///
  /// @returns      void
  virtual void Client_BufferLocalState();

  /// Applies the state previously buffered, restoring non-replicated state properties to their initial values.
  /// @see Client_BufferLocalState
  ///
  /// @returns      void
  virtual void Client_ApplyBufferedLocalState();

  /// Assures that all client actors and components that participate in the replication process tick in the correct order. To minimize
  /// dependencies this is checked every frame, a more efficient solution can easily be implemented in a more appropriate place (e.g. the
  /// game mode class).
  ///
  /// @returns      void
  virtual void Client_ManagePrerequisiteTicks();

  /// Returns the array with the client moves that are currently waiting to be sent to the server.
  ///
  /// @returns      const TArray<FMove>&    Reference-to-const to the array with the pending client moves.
  const TArray<FMove>& Client_GetPendingMoves() const;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Give authoritative power to the client regarding his location. If enabled, the server will set the actor location directly from the
  /// client value (if replicated) and will skip validation.
  /// ATTENTION: If this option is set to true, you should disable replication of the location for the server state of the autonomous proxy.
  /// Keep in mind that this has the effect that you can't change the location locally anymore inside the replicated tick function.
  /// 赋予客户端关于其位置的权威性。 如果启用，服务器将直接从客户端值（如果已复制）设置参与者位置，并将跳过验证。
  /// 注意：如果此选项设置为 true，则应禁用自主代理服务器状态的位置复制。 请记住，这具有您无法再在复制的刻度函数内本地更改位置的效果。
  bool bUseClientLocation{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Give authoritative power to the client regarding his rotation. If enabled, the server will set the actor rotation directly from the
  /// client values (if replicated) and will skip validation.
  /// ATTENTION: If this option is set to true, you should disable replication of all rotation components for the server state of the
  /// autonomous proxy. Keep in mind that this has the effect that you can't change the rotation locally anymore inside the replicated tick
  /// function.
  /// 赋予客户关于其旋转的权威性权力。 如果启用，服务器将直接从客户端值（如果复制）设置参与者旋转，并将跳过验证。
  /// 注意：如果此选项设置为 true，您应该为自主代理的服务器状态禁用所有轮换组件的复制。 请记住，这具有您无法再在复制的刻度函数内本地更改旋转的效果。
  bool bUseClientRotation{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Give authoritative power to the client regarding his control rotation. If enabled, the server will set the control rotation directly
  /// from the client values (if replicated) and will skip validation.
  /// ATTENTION: If this option is set to true, you should disable replication of all control rotation components for the server state of
  /// the autonomous proxy. Keep in mind that this has the effect that you can't change the control rotation locally anymore inside the
  /// replicated tick function.
  /// 赋予客户关于其控制器旋转的权威性权力。 如果启用，服务器将直接从客户端值（如果复制）设置其控制器旋转，并将跳过验证。
  /// 注意:如果将此选项设置为true，则应该禁用对自治代理的服务器状态的所有控制旋转组件的复制。请记住，这将导致您无法在复制的tick函数中在本地更改控制器旋转。
  bool bUseClientControlRotation{true};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "1", UIMax = "10"))
  /// Compare tolerance for the replication of the actor location to the server. If the difference between two location values is greater
  /// than this tolerance, the moves will not be combined.
  /// 比较actor位置复制到服务器的容差。 如果两个位置值之间的差异大于此容差，则不会合并移动。
  float LocationNetTolerance{5.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "1", UIMax = "30"))
  /// Compare tolerance for the replication of the actor rotation to the server. If the difference between two rotation values is greater
  /// than this tolerance, the moves will not be combined.
  /// 比较参与者轮换复制到服务器的容差。 如果两个旋转值之间的差异大于此容差，则不会合并移动。
  float RotationNetTolerance{10.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "1", UIMax = "30"))
  /// Compare tolerance for the replication of the control rotation to the server. If the difference between two control rotation values is
  /// greater than this tolerance, the moves will not be combined.
  /// 比较控制器旋转复制到服务器的容差。 如果两个控制器旋转值之间的差异大于此容差，则不会合并移动。
  float ControlRotationNetTolerance{10.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "0.04", UIMax = "0.2"))
  /// The maximum delta time allowed for a client move. If the time between two client moves is greater than this value, the delta time will
  /// be clamped (on the client AND the server). This means that the client movement will effectively slow down if his frame rate falls
  /// below 1 / MaxServerDeltaTime so this value should not be too low (but also not too high because then you may get a large performance
  /// hit from physics sub-stepping). Cannot be smaller than the inverse of the client send rate (if it is the value will be increased to
  /// match the client send interval i.e. 1 / client send rate).
  /// 客户端移动允许的最大增量时间。 如果两个客户端移动之间的时间大于此值，则增量时间将被限制（在客户端和服务器上）。
  /// 这意味着如果客户端的帧速率低于 1 / MaxServerDeltaTime，客户端移动将有效减慢，因此该值不应太低（但也不能太高，
  /// 因为这样您可能会从物理子步进中获得很大的性能影响）。 不能小于客户端发送速率的倒数（如果是，则该值将增加以匹配客户端发送间隔，即 1 / 客户端发送速率）。
  float MaxServerDeltaTime{0.05f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "0.01", UIMax = "0.2"))
  /// The maximum delta time allowed for a (combined) client move before it has to be enqueued in the move queue. In some situations you may
  /// want to enqueue moves more frequently so the server gets updates faster (the client send rate is still limited by how quickly we
  /// enqueue new moves). As this will not clamp the delta time of a client move, it can be set to small values without affecting the
  /// movement simulation (unlike the max server delta time). E.g. if you set this to 0.02 you will enqueue every move up to 50 fps. Cannot
  /// be greater than the max server delta time (will be lowered to match the max server delta time if it is).
  /// 在必须将其排入移动队列之前，允许（组合）客户端移动的最大增量时间。 在某些情况下，您可能希望更频繁地对移动进行排队，
  /// 以便服务器更快地获取更新（客户端发送速率仍然受到我们将新移动排队的速度限制）。 由于这不会限制客户端移动的增量时间，
  /// 因此可以将其设置为较小的值而不影响移动模拟（与最大服务器增量时间不同）。
  /// 例如, 如果您将其设置为 0.02，您将每次移动最多 50 fps。 不能大于最大服务器增量时间（如果是，将降低以匹配最大服务器增量时间）。
  float MaxClientDeltaTime{0.01f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "1", UIMin = "1", UIMax = "120"))
  /// How often per second the client will send data to the server. This is an upper bound i.e. the interval between two updates will never
  /// be lower than 1 / client send rate. However, more often than not it will be longer to save bandwidth if no important values changed on
  /// the client (moves will be combined). In this case the lower bound is determined by the max client delta time.
  /// 客户端每秒向服务器发送数据的频率。 这是一个上限，即两次更新之间的间隔永远不会低于 1 / 客户端发送速率。
  /// 但是，如果客户端上没有更改重要值（将合并移动），通常会更长时间节省带宽。 在这种情况下，下限由最大客户端增量时间确定。
  int32 ClientSendRate{100};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "2", UIMin = "32", UIMax = "256"))
  /// How many past moves the autonomous proxy is allowed to store at most. The appropriate value depends mostly on the network latency (a
  /// higher ping requires a larger move queue) but the net update frequency and client framerate need to be considered as well.
  /// 自治代理最多允许存储多少过去的移动。 适当的值主要取决于网络延迟（更高的 ping 需要更大的移动队列），但也需要考虑网络更新频率和客户端帧率。
  int32 MoveQueueMaxSize{64};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Only replay when moving. This can make corrections less noticeable for the client.
  /// 仅在移动时重播。 这可以使客户的更正不太明显。
  bool bOnlyReplayWhenMoving{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "0.1"))
  /// If the option to only replay when moving is enabled, this is the minimum velocity magnitude required before a replay is allowed to
  /// happen.
  /// 如果启用仅在移动时重放的选项，则这是允许重放发生之前所需的最小速度幅度。
  float ReplaySpeedThreshold{10.0f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "0.1", UIMax = "10"))
  /// How much the client velocity is allowed to deviate from the server calculated velocity to still be considered valid. The velocity is
  /// checked with every replication update on the client.
  /// 允许客户端速度偏离服务器计算速度多少仍被视为有效。 客户端上的每个复制更新都会检查速度。
  float MaxVelocityError{0.5f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "1", UIMax = "5"))
  /// How much the client location is allowed to deviate from the server calculated location to still be considered valid.
  /// 允许客户端位置偏离服务器计算位置多少仍被视为有效。
  float MaxLocationError{1.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "1", UIMax = "5"))
  /// How much the client actor rotation is allowed to deviate from the server calculated actor rotation to still be considered valid.
  /// 允许客户端 Actor 旋转 偏离服务器计算的 Actor 旋转多少仍然被认为是有效的。
  float MaxRotationError{5.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0.000001", UIMin = "1", UIMax = "5"))
  /// How much the client control rotation is allowed to deviate from the server calculated control rotation to still be considered valid.
  /// 允许客户端控制器旋转与服务器计算的控制器旋转偏离多少仍被视为有效。
  float MaxControlRotationError{5.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0", UIMin = "0.01", UIMax = "0.05"))
  /// Only relevant for simulated proxy server data. This determines how long a variable that is configured to force a net update upon
  /// changing should persist within the server state when it has just changed, to ensure that the value will actually be replicated before
  /// it changes again. Example with a boolean variable and a min hold time of 0.01: When the value of the bool changes from false to true
  /// on the server, it will be locked for 10 ms and will ignore any further changes within that time frame. If this feature is disabled
  /// (i.e. min hold time is set to 0) and the bool changes immediately back to false again with the next move, the value "true" might not
  /// be replicated to other clients.
  /// 仅与模拟代理服务器数据相关。 这决定了配置为在更改时强制进行网络更新的变量在刚刚更改时应该在服务器状态中保留多长时间，以确保在再次更改之前实际复制该值。
  /// 布尔变量和最小保持时间为 0.01 的示例：当 bool 的值在服务器上从 false 更改为 true 时，它将被锁定 10 ms，并且将忽略该时间范围内的任何进一步更改。
  /// 如果禁用此功能（即最小保持时间设置为 0），并且 bool 会在下一次移动时立即再次变为 false，则值“true”可能不会复制到其他客户端。
  float MinRepHoldTime{0.02f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0", UIMin = "0", UIMax = "20"))
  /// If greater than 0 the server will fully serialize all data anew every "FullSerializationInterval" seconds. This is intended as a sort
  /// of fail-safe mechanism for pawn data replication. Only activate this if you notice client pawns remaining stuck in a wrong state in
  /// stand-alone builds (very rare and application specific).
  /// 如果大于 0，服务器将每隔“FullSerializationInterval”秒重新完全序列化所有数据。 这是一种用于Pawn数据复制的故障安全机制。
  /// 仅当您注意到客户端 pawn 在 stand-alone 构建中仍处于错误状态时才激活此功能（非常罕见且特定于应用程序）。
  float FullSerializationInterval{0.f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When enabled, the client will always replay its state upon receiving a server update. Enabling may cause a large performance hit and
  /// can sometimes introduce instabilities in the client state that would otherwise not occur. This option should generally be disabled
  /// except for testing purposes.
  /// 启用后，客户端将始终在收到服务器更新时重播其状态。
  /// 启用可能会导致较大的性能损失，并且有时会在客户端状态中引入不稳定性，否则不会发生。
  /// 此选项通常应禁用，除非出于测试目的。
  bool bAlwaysReplay{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Only relevant for listen servers. Whether pawns on a listen server that are remotely controlled by a client should be smoothed. This
  /// essentially allows you to see how movement would happen on a dedicated server as these will never run any interpolation logic.
  /// Disabling this is not the same as setting the interpolation method to "None", and some animations/effects might not play. Should only
  /// be disabled for testing purposes.
  bool bSmoothRemoteServerPawn{true};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay , meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When set to false, only the location of the children of the root component will be smoothed, the location of the collision will always
  /// be set directly to the target state of the interpolation. This will prevent erroneous collisions from remotely controlled pawns on the
  /// local machine due to smoothing. Note that when set to true the collision location will also be smoothed when extrapolating. It is
  /// recommended to keep this disabled.
  bool bSmoothCollisionLocation{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay , meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When set to false, only the rotation of the children of the root component will be smoothed, the rotation of the collision will always
  /// be set directly to the target state of the interpolation. This will prevent erroneous collisions from remotely controlled pawns with
  /// non-symmetric collision shapes on the local machine due to smoothing. Note that when set to true the collision rotation will also be
  /// smoothed when extrapolating. It is recommended to keep this disabled.
  bool bSmoothCollisionRotation{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When enabled, the remote move passed for move execution on the server will never contain any NAN values. Data that the client does not
  /// replicate to the server will just be 0 instead.
  bool bEnsureValidMoveData{true};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When enabled, bandwidth usage is reduced when replicating the server state to clients. Specifically, network traffic is optimized
  /// by comparing the last serialized value to the one that is about to be replicated: if it has changed it is serialized anew, otherwise
  /// only 1 bit is replicated to indicate to the client that the last received value can be used again. Should generally always be enabled
  /// except for testing purposes.
  bool bOptimizeTraffic{true};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When enabled, the control rotation will be quantized locally after move execution as well. This is usually not needed because often
  /// the control rotation does not affect collision (or may not be replicated at all), and there is a slight chance that quantization could
  /// be detrimental to the gameplay experience (e.g. it could make aiming feel inconsistent). However, if you replicate and replay the
  /// client's control rotation or it alters the pawn's collision, you can enable this.
  bool bQuantizeControlRotation{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// How strongly the location of the pawn should be compressed.
  EDecimalQuantization LocationQuantize{EDecimalQuantization::RoundTwoDecimals};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// How strongly the velocity of the pawn should be compressed.
  EDecimalQuantization VelocityQuantize{EDecimalQuantization::RoundTwoDecimals};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// How strongly the rotation of the pawn should be compressed. WARNING: Do not use byte compression.
  ESizeQuantization RotationQuantize{ESizeQuantization::Short};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// How strongly the control rotation of the pawn should be compressed. WARNING: Do not use byte compression.
  ESizeQuantization ControlRotationQuantize{ESizeQuantization::Short};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Verification", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Whether the server should verify the timestamps of the movement data received from the client. This increases security at the cost of
  /// some processing power.
  bool bVerifyClientTimestamps{false};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Verification", meta =
    (ClampMin = "0", UIMin = "0.04", UIMax = "0.12", EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// The maximum amount in seconds a client timestamp is allowed to differ from the server calculated one to still be considered valid.
  /// The appropriate value here depends on a lot of variables, try to find the lowest value that does not fail too many verifications.
  float MaxAllowedTimestampDeviation{0.08f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Verification", meta =
    (ClampMin = "0", UIMin = "0", UIMax = "10", EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// The maximum number of strikes a client is allowed to receive within the time interval of two resets. If this number is exceeded moves
  /// will be rejected immediately for the remainder of the interval whenever they fail to verify.
  int32 MaxStrikeCount{2};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Verification", meta =
    (ClampMin = "0", UIMin = "1", UIMax = "60", EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// The time interval for resetting the client strike count.
  float StrikeResetInterval{10.f};

#pragma endregion

#pragma region Smoothing

private:

  using InterpolationFunctionPtr = FState(UGenMovementReplicationComponent::*)(
    const FState& StartState,
    const FState& TargetState,
    float InterpolationRatio
  ) const;

  /// Pointer to the currently set interpolation function.
  InterpolationFunctionPtr InterpolationFunction{nullptr};

  /// Stores the server states for local movement simulation of remotely controlled pawns. New states are appended at the end of the queue,
  /// i.e. the smaller the index the older the state is.
  TArray<FState> StateQueue;

  /// Checked against to see if the current interpolation time is valid. It might not be after the world time was synchronised on a client
  /// or immediately after the world was brought up.
  float LastValidInterpolationTime{0.f};

  /// Holds the state that was used as the start state during the last interpolation.
  FState InterpolationStartState;

  /// Holds the state that was used as the target state during the last interpolation.
  FState InterpolationTargetState;

  /// Holds the state data that resulted from the last interpolation.
  FState InterpolatedState;

  /// Holds the state data that resulted from the last extrapolation. We use this state as the start state once we can interpolate again
  /// with server data to smooth corrections out.
  FState ExtrapolatedState;

  /// True if we are currently running the interpolation function with a ratio > 1 or if we interpolating with an extrapolated state as
  /// start state. This can be true even when @see bAllowExtrapolation is set to false and we are using an interpolation function because
  /// during latency spikes the pawn state is technically still interpolated between two states, they just happen to be same state. The
  /// timestamp will be updated and used in the same way it is when extrapolation is allowed.
  bool bUsingExtrapolatedData{false};

  /// The index in the state queue of the start state used during the last interpolation. Passed to @see SimulatedTick. Will be -1 if no
  /// valid start state is currently available.
  int32 CurrentStartStateIndex{-1};

  /// The index in the state queue of the target state used during the last interpolation. Passed to @see SimulatedTick. Will be -1 if no
  /// valid target state is currently available.
  int32 CurrentTargetStateIndex{-1};

  /// The timestamp of the state that was the target state for interpolation during the last tick. Used by @see DetermineSkippedStates to
  /// check whether any states have been omitted from interpolation by the smoothing algorithm during the current tick.
  float PreviousInterpolationTargetStateTimestamp{0};

  /// Contains the replicated control rotation for simulated proxies since they do not have a controller on the client. This is the same
  /// value as the control rotation of the smooth state passed to the simulated tick function, but already transformed into the local space
  /// of the pawn's root collision component. Only valid to use for pawns with the net role ROLE_SimulatedProxy.
  FRotator SmoothedControlRotation{0};

  /// Generic function for smoothing out the local simulation of remotely controlled pawns. Performs inter- or extrapolation from the state
  /// queue data based on the set simulation delay.
  ///
  /// @param        Context    The context in which the pawn is being smoothed.
  /// @returns      void
  void SmoothMovement(ESimulatedContext Context);

  /// Adds an additional component to the pawn's scene component hierarchy just underneath the root component, which is then used to move
  /// the other attached components independently of the root collision during smoothing.
  ///
  /// @returns      void
  void AddSimulatedRootComponent();

  /// Removes a previously added simulated root component from the pawn's scene component hierarchy again.
  ///
  /// @returns      void
  void RemoveSimulatedRootComponent();

  /// Determines the appropriate data for interpolation (or extrapolation) from the state queue based on the passed interpolation time.
  ///
  /// @param        Time                     The current interpolation time.
  /// @param        OutStartState            The start state that should be used for interpolation.
  /// @param        OutTargetState           The target state that should be used for interpolation.
  /// @param        OutInterpolationRatio    The ratio to be used for interpolation.
  /// @returns      void
  void ComputeSmoothingInput(float Time, FState& OutStartState, FState& OutTargetState, float& OutInterpolationRatio);

  /// Interpolates via the set interpolation method between the start and target state based on the passed interpolation ratio and fills
  /// "SmoothState" with the calculated velocity, location, rotation and control rotation values. Also does extrapolation if the passed
  /// interpolation ratio is greater than 1.
  ///
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states.
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        SmoothState           The new interpolated state.
  /// @returns      void
  void ComputeInterpolatedState(float InterpolationRatio, const FState& StartState, const FState& TargetState, FState& SmoothState) const;

  /// Determines the indices of states that have been omitted from interpolation since the last simulated tick. Ideally, the smoothing
  /// algorithm progresses uniformly through the saved states, meaning the current start state was the previous target state and the current
  /// target state will be the next start state and so on. However, this cannot be guaranteed so this function checks whether we have
  /// skipped any states during smoothing.
  ///
  /// @param        OutSkippedStateIndices    The indices of all states (ordered newest to oldest) that have been omitted from interpolation
  ///                                         between the current and the last simulated tick. Will be empty if no states have been skipped.
  /// @returns      void
  void DetermineSkippedStates(TArray<int32>& OutSkippedStateIndices) const;

  /// Adds a state to the end of the state queue. Automatically manages queue size and deletes the oldest state if the max queue size was
  /// reached.
  ///
  /// @param        State    The state to add to the state queue.
  /// @returns      bool     True if the state was added, false if not.
  bool AddToStateQueue(const FState& State);

  /// Used in connection with @see SetPawnState to specify which values to update.
  enum EStateUpdate : uint8
  {
    UpdateVelocity              = 0x01,
    UpdateLocationRoot          = 0x02,
    UpdateLocationSimulatedRoot = 0x04,
    UpdateRotationRoot          = 0x08,
    UpdateRotationSimulatedRoot = 0x10,
    UpdateControlRotation       = 0x20,
    UpdateAll                   = 0xFF,
  };

  /// Updates the pawn state from the values of the passed state in accordance with the passed bit flag.
  ///
  /// @param        State             The state the pawn should adopt.
  /// @param        ValuesToUpdate    Bit flag to determine which values to update (@see EStateUpdate).
  /// @returns      void
  void SetPawnState(const FState& State, uint8 ValuesToUpdate);

  /// Transforms the replicated control rotation in world space into the local space of the pawn's root collision component (which is more
  /// convenient for animation purposes).
  /// @attention Keep in mind that the transformation introduces a small error.
  ///
  /// @param        WorldControlRotation    The replicated control rotation in world space.
  /// @returns      FRotator                The control rotation in the local space of the pawn's root collision component.
  FRotator ControlRotationToLocal(const FRotator& WorldControlRotation) const;

  /// Initializes the state to be interpolated with either the start or the target state, depending on which we are closer to in time based
  /// on the configured interpolation delay. This is only relevant for non-interpolated members (e.g. booleans). We also ensure that we
  /// don't choose a state for initialization with which we would skip a value that forces a net update.
  ///
  /// @param        Time           The interpolation time to use.
  /// @param        StartState     The start state for the interpolation.
  /// @param        TargetState    The target state for the interpolation.
  /// @returns      FState         The initialization state.
  FState CreateInitializationState(float Time, const FState& StartState, const FState& TargetState) const;

  /// Some operations that may be used in the interpolation function don't preserve NaNs (e.g. converting FRotator to FQuat). This method
  /// restores the NaNs in the interpolated state if they were also present in the source state.
  ///
  /// @param        SmoothState    The interpolated state that may not contain the original NaN values anymore.
  /// @param        SourceState    The state (usually the start or target state of the interpolation) that still contains the NaNs.
  /// @returns      void
  void InterpolatedStatePreserveNaN(FState& SmoothState, const FState& SourceState) const;

protected:

  /// Performs linear interpolation between the start and target state based on the passed interpolation ratio.
  ///
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states. Performs extrapolation if > 1.
  /// @returns      FState                The resulting interpolated state.
  virtual FState InterpolateLinear(const FState& StartState, const FState& TargetState, float InterpolationRatio) const;

  /// Performs cubic interpolation between the start and target state based on the passed interpolation ratio. The main difference to linear
  /// interpolation is that this also considers the velocity of the pawn which can improve the simulation for applications that use a more
  /// physically based movement system (e.g. vehicles).
  ///
  /// @param        StartState            The start state for the interpolation.
  /// @param        TargetState           The target state for the interpolation.
  /// @param        InterpolationRatio    The percentage at which to interpolate between the two states. Performs extrapolation if > 1.
  /// @returns      FState                The resulting interpolated state.
  virtual FState InterpolateCubic(const FState& StartState, const FState& TargetState, float InterpolationRatio) const;

  /// Constructs a new FState from the passed (interpolated) values. Intended to be used in Blueprint to create the return value when
  /// overriding a custom interpolation function.
  ///
  /// @param        FinalLocation           The final/interpolated location calculated by the custom interpolation function.
  /// @param        FinalVelocity           The final/interpolated velocity calculated by the custom interpolation function.
  /// @param        FinalRotation           The final/interpolated actor rotation calculated by the custom interpolation function.
  /// @param        FinalControlRotation    The final/interpolated control rotation calculated by the custom interpolation function.
  /// @returns      FState                  The new state with the interpolated values.
  UFUNCTION(BlueprintCallable, Category = "General Movement Component")
  FState MakeFState(
    const FVector& FinalLocation,
    const FVector& FinalVelocity,
    const FRotator& FinalRotation,
    const FRotator& FinalControlRotation
  ) const;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Smoothing", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0", UIMin = "0.01", UIMax = "2"))
  /// How far back in time (in seconds) remotely controlled pawns are displayed on the local machine. Lower values are obviously more
  /// "realtime" but may produce unsatisfactory results with bad network conditions. Larger values enable a greater tolerance for packet
  /// loss, latency spikes and higher ping but remotely controlled pawns are farther in the past on the local machine. In general, the
  /// better you expect network conditions to be the lower you can set this value. Keep in mind that this system is based on the world time
  /// of the machine where the original move was created. You need to consider the longest time it can take for one packet to get from one
  /// client to another (i.e. the two highest client pings added up). When running a listen server that accepts only one client connection,
  /// you only have to consider the latency of that one connection which will reduce the required simulation delay quite a bit in most
  /// circumstances. For advanced users: the simulation delay is a local property and it is possible to configure this individually for
  /// every machine at runtime based on ping, but be aware that this may break server pawn rollback if not handled properly.
  /// ATTENTION: The network presets are tailored towards servers that accept only one client connection (e.g. co-op games where one player
  /// hosts as a listen server and only one other player can join as a client). If you want to support multiple client connections, an
  /// estimation for an appropriate simulation delay would be to double whatever the value of the current preset is. You can also change the
  /// simulation delay for all remotely controlled pawns on a machine directly in-game by using the "SetInterpolationDelay" console command
  /// (may break server pawn rollback). The simulation delay is only relevant when the interpolation method is not "None".
  float SimulationDelay{0.15f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Smoothing", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// The method of interpolation used to smooth the movement of remotely controlled pawns. If set to "None" interpolation is disabled
  /// and pawns will always be set to their most recently received server state (completely ignoring the simulation delay) which assures the
  /// lowest latency possible but may cause jittery visuals.
  EInterpolationMethod InterpolationMethod{EInterpolationMethod::Linear};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Smoothing", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom", ClampMin = "0", UIMin = "32", UIMax = "512"))
  /// The max size of the array containing the saved pawn states for interpolation and rollback i.e. how many past states we want to store
  /// before starting to delete the oldest ones. Greater network latency requires a larger state queue, as does a larger simulation delay
  /// (other factors may need to be considered as well).
  int32 StateQueueMaxSize{128};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking|Smoothing", meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// Whether extrapolation is allowed when no recent enough state data is available. At best this can cover up small lag spikes without any
  /// noticeable disruption. At worst, it can cause severe rubberbanding and possibly pawns going through blocking objects.
  bool bAllowExtrapolation{false};

#pragma endregion

#pragma region Rollback

private:

  /// Used to buffer the local state of server pawns before the rollback. On the server, saving the full state of a pawn is the easiest way
  /// to restore it because the latest state queue entry might not always accurately reflect the local state (as it is geared towards
  /// replication, e.g. some bound data might be locked and still hold older values).
  FState Server_RestoreState;

  /// Returns a list of all pawns that should be considered for rollback.
  ///
  /// @returns      TArray<AGenPawn*>    Contains all pawns that should be rolled back.
  TArray<AGenPawn*> GatherRollbackPawns() const;

  /// Tests whether the passed pawn is a candidate for rollback.
  ///
  /// @param        PawnToTest    The pawn that should be tested.
  /// @returns      bool          True if the passed pawn should be rolled back, false otherwise.
  bool ShouldBeRolledBack(const APawn* PawnToTest) const;

  /// Sets all other pawns back to their past states when moves are executed either on the server for simulating clients or in a client
  /// replay context. When a client executes a move locally the state of its world most likely differs from the state of the server world
  /// when the move is executed there or from the state of its own world when a replay is triggered, meaning remotely controlled pawns are
  /// not in the same positions as they were when the move was originally executed. This can lead to rejected client moves on the server
  /// and faulty replays on the client. If @see bRollbackServerPawns and/or @see bRollbackClientPawns is true and moves are executed in the
  /// aforementioned contexts, all other pawns are set back to the appropriate past state from their state queue (based on the configured
  /// simulation delay) so that, while simulating, the current world resembles the client world more closely at the time the client
  /// originally executed the moves.
  ///
  /// @param        Time               The time to set the pawns back to, usually the move time minus the configured simulation delay.
  /// @param        PawnsToRollBack    All pawns that should be rolled back.
  /// @param        Context            The context in which the pawns are being rolled back.
  /// @returns      void
  void RollbackPawns(float Time, const TArray<AGenPawn*>& PawnsToRollBack, ESimulatedContext Context) const;

  /// Finds the start and target states for rollback in the passed queue and calculates the interpolation ratio to use.
  ///
  /// @param        Time                     The timestamp to search for.
  /// @param        StateQueueToSearch       The queue to search.
  /// @param        OutStartState            The found start state.
  /// @param        OutTargetState           The found target state.
  /// @param        OutInterpolationRatio    The ratio to be used for interpolation. Will be -1 if no states were found to interpolate.
  /// @returns      bool                     False if no start and/or target state could be found in the passed queue, true otherwise.
  bool ComputeRollbackInput(
    float Time,
    const TArray<FState>& StateQueueToSearch,
    FState& OutStartState,
    FState& OutTargetState,
    float& OutInterpolationRatio
  ) const;

  /// After move execution finished in a context where a rollback happened, this function sets all rolled back pawns back to their intial
  /// (i.e. most current) states.
  ///
  /// @param        PawnsToRestore    List containing all pawns that were rolled back and whose states should be restored.
  /// @returns      void
  void RestoreRolledBackPawns(const TArray<AGenPawn*>& PawnsToRestore) const;

protected:

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When enabled, all other pawns of type AGenPawn (with a controller) are set back in time for move execution on the server, so the
  /// server world resembles the client world more closely at the point in time when the client originally executed the move. This can
  /// significantly improve simulation accuracy, but comes at the cost of an increased server performance overhead, especially when many
  /// clients are connected. For this to produce good results, all smoothing parameters ("SimulationDelay", "InterpolationMethod",
  /// "SmoothCollisionLocation", "SmoothCollisionRotation") must be synchronised between server and client for any pawn that is rolled back.
  /// Additionally, pawns will only participate in the rollback if their interpolation method is not set to "None".
  /// ATTENTION: Only values that are actually serialized and sent to the server by the client are rolled back.
  bool bRollbackServerPawns{true};

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Networking", AdvancedDisplay, meta =
    (EditCondition = "NetworkPreset == ENetworkPreset::Custom"))
  /// When enabled, all simulated proxies of type AGenPawn are set back in time for replay on the client. This can significantly improve
  /// replay accuracy (thus making corrections less noticable) at the cost of an increased client performance overhead. Pawns will only
  /// participate in the rollback if their interpolation method is not set to "None".
  /// ATTENTION: Only values that are actually serialized and replicated to the client from the server are rolled back.
  bool bRollbackClientPawns{true};

#pragma endregion

#pragma region Input IDs

  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID MoveForward"))
  FName ID_MoveForward() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_MoveForward; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID MoveRight"))
  FName ID_MoveRight() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_MoveRight; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID MoveUp"))
  FName ID_MoveUp() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_MoveUp; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID RollView"))
  FName ID_RollView() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_RollView; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID PitchView"))
  FName ID_PitchView() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_PitchView; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID TurnView"))
  FName ID_TurnView() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_TurnView; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID RollViewRate"))
  FName ID_RollViewRate() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_RollViewRate; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID PitchViewRate"))
  FName ID_PitchViewRate() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_PitchViewRate; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Axis Mappings", meta = (CompactNodeTitle = "ID TurnViewRate"))
  FName ID_TurnViewRate() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_TurnViewRate; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 1"))
  FName ID_Action1() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action1; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 2"))
  FName ID_Action2() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action2; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 3"))
  FName ID_Action3() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action3; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 4"))
  FName ID_Action4() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action4; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 5"))
  FName ID_Action5() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action5; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 6"))
  FName ID_Action6() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action6; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 7"))
  FName ID_Action7() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action7; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 8"))
  FName ID_Action8() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action8; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 9"))
  FName ID_Action9() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action9; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 10"))
  FName ID_Action10() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action10; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 11"))
  FName ID_Action11() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action11; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 12"))
  FName ID_Action12() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action12; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 13"))
  FName ID_Action13() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action13; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 14"))
  FName ID_Action14() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action14; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 15"))
  FName ID_Action15() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action15; } return {}; }
  UFUNCTION(BlueprintPure, Category = "Input|Action Mappings", meta = (CompactNodeTitle = "ID Action 16"))
  FName ID_Action16() { if (const auto Owner = Cast<AGenPawn>(GetOwner())) { return Owner->ID_Action16; } return {}; }

#pragma endregion

#pragma region Input Flag Binding

  /// The input flags are handled differently from the other pre-replicated data as they represent actual (physical) user input that is
  /// replicated from client to server, whereas the other data is just calculated within the tick function as a result of that input and is
  /// replicated from server to client. Input flags always force a net update upon changing and are never replicated back to the autonomous
  /// proxy.
  /// 输入标志的处理方式不同于其他预复制的数据，因为它们代表从客户端复制到服务器的实际（物理）用户输入，
  /// 而其他数据只是在tick函数中作为该输入的结果进行计算，并从服务器复制到客户端。输入标志总是在更改时强制进行网络更新，并且永远不会复制回自治代理。
  /// 
  /// @attention At the time of binding Blueprint variables are not initialized yet (meaning you should not pass editor properties as
  /// function arguments).
  /// 在绑定Blueprint变量时，还没有初始化(这意味着您不应该将编辑器属性作为函数参数传递)。
  ///
  /// @param        ActionName                    The name of the input action that should be bound to the data member.
  ///                                             应该绑定到数据成员的输入操作的名称。
  /// @param        VariableToBind                The variable to bind to an input flag.
  ///                                             要绑定到输入标志的变量。
  /// @param        bReplicateToSimulatedProxy    Whether the bound variable should be replicated to simulated proxies. Usually this should
  ///                                             only be true for variables that trigger animations that remote clients should also be
  ///                                             able to see. Also affects smoothed listen server pawns in the same way.
  ///                                             绑定变量是否应复制到模拟代理。 通常这应该只适用于触发远程客户端也应该能够看到的动画的变量。
  ///                                             也以相同的方式影响平滑的监听服务器棋子。
  /// @param        bNoMoveCombine                Whether this input flag should always cause a new move to be enqueued while it is set
  ///                                             (only relevant for autonomous proxies).
  ///                                             此输入标志是否应始终在设置时导致新移动排队（仅与自治代理相关）。
  /// @returns      void
public:

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindInputFlag",
    ToolTip = "Bind a boolean value to the input action with the specified name. The flag will be set while the input is being triggered.\n将布尔值绑定到具有指定名称的输入操作。 该标志将在输入被触发时设置。"))
  void K2_BindInputFlag(
    FName ActionName,
    UPARAM(ref) bool& VariableToBind,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Do Not Combine") bool bNoMoveCombine = false
  );

  void BindInputFlag(
    FName ActionName,
    bool& VariableToBind,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Do Not Combine") bool bNoMoveCombine = false
  );

protected:

  virtual void Server_SwapStateBufferBoundInputFlags();
  virtual bool Server_ForceNetUpdateCheckBoundInputFlags();
  virtual void Server_SaveBoundInputFlagsToServerState(FState& ServerState, const FMove& SourceMove) const;
  virtual void Server_ResetLockedBoundInputFlags();
  virtual bool Client_EnqueueMoveCheckBoundInputFlags(const FMove& CurrentMove) const;
  virtual void SaveBoundInputFlagsToMove(FMove& Move) const;
  virtual void SaveBoundInputFlagsToState(FState& State) const;
  virtual void LoadBoundInputFlagsFromMove(const FMove& Move) const;
  virtual void LoadBoundInputFlagsFromState(const FState& State) const;
  virtual void LoadReplicatedBoundInputFlagsFromState(const FState& State) const;
  virtual void AddTargetStateInputFlagsToInitState(FState& InitializationState, const FState& TargetState) const;
  virtual void AddStartStateInputFlagsToInitState(FState& InitializationState, const FState& StartState) const;
  DEFINE_PREREPLICATED_INPUT_DATA()

#pragma endregion

#pragma region Pre-replicated Data

  ///***************************************************************************************************************************************
  /// The variables provided here can be bound to user-defined members through the binding functions. Bound values are processed
  /// automatically by the replication component meaning they can be move-combined, replayed and carried over between ticks without any
  /// additional implementation by the user. They will not be sent from client to server, but the server will replicate them to clients
  /// according to the configuration. As a general guideline the autonomous proxy should have all bound members replicated (for replays),
  /// whereas simulated proxies can have them replicated as required (usually for animation purposes).
  /// 这里提供的变量可以通过绑定函数绑定到用户定义的成员。
  /// 绑定值由复制组件自动处理，这意味着它们可以在Tick之间进行移动组合、重放和转移，而无需用户进行任何额外的实现。
  /// 它们不会从客户端发送到服务器，但服务器会根据配置将它们复制到客户端。
  /// 作为一般准则，自治代理应该复制所有绑定成员（用于重播），而模拟代理可以根据需要复制它们（通常用于动画目的）。
  /// 
  /// @attention Any members that are not bound to one of these variables should not carry over data between ticks or be move-combined.
  ///            任何未绑定到这些变量之一的成员不应在Tick之间传递数据或进行移动组合。
  /// @attention Do not modify bound data members outside of the replicated tick.
  ///            不要在复制的Tick之外修改绑定的数据成员。
  ///***************************************************************************************************************************************

  /// Extendable binding interface for Blueprint.
  /// 蓝图的可扩展绑定接口。
  /// 
  /// @attention At the time of binding Blueprint variables are not initialized yet (meaning you should not pass editor properties as
  /// function arguments).
  /// 在绑定Blueprint变量时，还没有初始化(这意味着您不应该将编辑器属性作为函数参数传递)。
  /// 
  /// @attention Binding a variable and setting both "bReplicateToAutonomousProxy" and "bReplicateToSimulatedProxy" to false does not have
  /// the same effect as not binding the variable at all. Binding integrates the data member into many internal processes and there are more
  /// advanced use cases where certain optimizations can be done by binding a variable but not replicating it.
  /// 绑定变量并将“bReplicateToAutonomousProxy”和“bReplicateToSimulatedProxy”都设置为 false 与根本不绑定变量的效果不同。
  /// 绑定将数据成员集成到许多内部流程中，并且有更高级的用例可以通过绑定变量而不是复制变量来完成某些优化。
  ///
  /// @param        VariableToBind                 The variable to bind to the pre-replicated data type.
  ///                                              要绑定到预复制数据类型的变量。
  /// @param        bReplicateToAutonomousProxy    Whether the bound variable should be replicated to the autonomous proxy. Save for some
  ///                                              advanced cases, this should generally be set to true because bound variables are usually
  ///                                              required for the client replay.
  ///                                              绑定变量是否应复制到自治代理。除某些高级情况外，这通常应设置为 true，因为客户端重播通常需要绑定变量。
  /// @param        bReplicateToSimulatedProxy     Whether the bound variable should be replicated to simulated proxies. Usually this should
  ///                                              only be true for variables that trigger animations that remote clients should also be
  ///                                              able to see. Also affects smoothed listen server pawns in the same way.
  ///                                              绑定变量是否应复制到模拟代理。通常这应该只适用于触发远程客户端也应该能够看到的动画的变量。
  ///                                              也以相同的方式影响平滑的监听服务器Pawn。
  /// @param        bForceNetUpdateOnChange        Only relevant when bReplicateToSimulatedProxy is true. Whether a net update should be
  ///                                              forced to simulated proxies every time the value of the bound variable changes. This
  ///                                              should generally only be enabled for variables that change infrequently and for which
  ///                                              remote clients need to know about every intermediate value. Net updates are never forced
  ///                                              to the autonomous proxy from bound data.
  ///                                              仅当 bReplicateToSimulatedProxy 为真时才相关。
  ///                                              每次绑定变量的值发生变化时，是否应强制对模拟代理进行净更新。
  ///                                              这通常应该只对不经常更改的变量启用，并且远程客户端需要知道每个中间值。
  ///                                              网络更新永远不会从绑定数据强制到自治代理。
  /// @returns      void
public:

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindBool",
    ToolTip = "Bind a regular boolean value. Uses only 1 bit for replication. 绑定一个常规的布尔值, 仅使用 1 bit进行复制。"))
  void K2_BindBool(
    UPARAM(Ref) bool& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindBool(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindHalfByte",
    ToolTip = "Bind an unsigned 8 bit integer that only has its lower 4 bits replicated."))
  void K2_BindHalfByte(
    UPARAM(ref) uint8& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindHalfByte(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindByte",
    ToolTip = "Bind a regular unsigned 8 bit integer."))
  void K2_BindByte(
    UPARAM(ref) uint8& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindByte(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindInt",
    ToolTip = "Bind a regular 4 byte integer."))
  void K2_BindInt(
    UPARAM(ref) int32& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindInt(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindFloat",
    ToolTip = "Bind a regular 4 byte float."))
  void K2_BindFloat(
    UPARAM(ref) float& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindFloat(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindVector",
    ToolTip = "Bind a quantized FVector. Replicated with 2 decimal places of precision (up to 30 bits per component)."))
  void K2_BindVector(
    UPARAM(ref) FVector& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindVector(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindNormal",
    ToolTip = "Bind a quantized FVector with a valid range of -1 to +1 per component. Uses 3 x 16 bit for replication (4 decimal places of precision)."))
  void K2_BindNormal(
    UPARAM(ref) FVector& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindNormal(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindRotator",
    ToolTip = "Bind a quantized FRotator. Uses 16 bit per component for replication (2 decimal places of precision)."))
  void K2_BindRotator(
    UPARAM(ref) FRotator& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindRotator(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindActorReference",
    ToolTip = "Bind a reference to any object of type AActor."))
  void K2_BindActorReference(
    UPARAM(ref) AActor*& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindActorReference(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindActorComponentReference",
    ToolTip = "Bind a reference to any object of type UActorComponent."))
  void K2_BindActorComponentReference(
    UPARAM(ref) UActorComponent*& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindActorComponentReference(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindAnimMontageReference",
    ToolTip = "Bind a reference to any object of type UAnimMontage."))
  void K2_BindAnimMontageReference(
    UPARAM(ref) UAnimMontage*& VariableToBind,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindAnimMontageReference(VariableToBind, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  /// Binding functions with accessors. These work the same as the regular binding functions but provide an additional out-parameter that
  /// can be used to access the bound variable from a state independently of the order in which the data was bound.
  ///
  /// @param        VariableToBind                 The variable to bind to the pre-replicated data type.
  /// @param        OutAccessor                    The internal variable name to which the passed variable was bound.
  /// @param        bReplicateToAutonomousProxy    Whether the bound variable should be replicated to the autonomous proxy. Save for some
  ///                                              advanced cases, this should generally be set to true because bound variables are usually
  ///                                              required for the client replay.
  /// @param        bReplicateToSimulatedProxy     Whether the bound variable should be replicated to simulated proxies. Usually this should
  ///                                              only be true for variables that trigger animations that remote clients should also be
  ///                                              able to see. Also affects smoothed listen server pawns in the same way.
  /// @param        bForceNetUpdateOnChange        Only relevant when bReplicateToSimulatedProxy is true. Whether a net update should be
  ///                                              forced to simulated proxies every time the value of the bound variable changes. This
  ///                                              should generally only be enabled for variables that change infrequently and for which
  ///                                              remote clients need to know about every intermediate value. Net updates are never forced
  ///                                              to the autonomous proxy from bound data.
  /// @returns      void
public:

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindBoolWithAccessor",
    ToolTip = "Bind a regular boolean value. Uses only 1 bit for replication."))
  void K2_BindBoolWithAccessor(
    UPARAM(Ref) bool& VariableToBind,
    UPARAM(Ref) EPreReplicatedBool& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindBoolWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindHalfByteWithAccessor",
    ToolTip = "Bind an unsigned 8 bit integer that only has its lower 4 bits replicated."))
  void K2_BindHalfByteWithAccessor(
    UPARAM(ref) uint8& VariableToBind,
    UPARAM(Ref) EPreReplicatedHalfByte& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindHalfByteWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindByteWithAccessor",
    ToolTip = "Bind a regular unsigned 8 bit integer."))
  void K2_BindByteWithAccessor(
    UPARAM(ref) uint8& VariableToBind,
    UPARAM(Ref) EPreReplicatedByte& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindByteWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindIntWithAccessor",
    ToolTip = "Bind a regular 4 byte integer."))
  void K2_BindIntWithAccessor(
    UPARAM(ref) int32& VariableToBind,
    UPARAM(Ref) EPreReplicatedInt& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindIntWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindFloatWithAccessor",
    ToolTip = "Bind a regular 4 byte float."))
  void K2_BindFloatWithAccessor(
    UPARAM(ref) float& VariableToBind,
    UPARAM(Ref) EPreReplicatedFloat& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindFloatWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindVectorWithAccessor",
    ToolTip = "Bind a quantized FVector. Replicated with 2 decimal places of precision (up to 30 bits per component)."))
  void K2_BindVectorWithAccessor(
    UPARAM(ref) FVector& VariableToBind,
    UPARAM(Ref) EPreReplicatedVector& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindVectorWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindNormalWithAccessor",
    ToolTip = "Bind a quantized FVector with a valid range of -1 to +1 per component. Uses 3 x 16 bit for replication (4 decimal places of precision)."))
  void K2_BindNormalWithAccessor(
    UPARAM(ref) FVector& VariableToBind,
    UPARAM(Ref) EPreReplicatedNormal& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindNormalWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindRotatorWithAccessor",
    ToolTip = "Bind a quantized FRotator. Uses 16 bit per component for replication (2 decimal places of precision)."))
  void K2_BindRotatorWithAccessor(
    UPARAM(ref) FRotator& VariableToBind,
    UPARAM(Ref) EPreReplicatedRotator& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindRotatorWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindActorReferenceWithAccessor",
    ToolTip = "Bind a reference to any object of type AActor."))
  void K2_BindActorReferenceWithAccessor(
    UPARAM(ref) AActor*& VariableToBind,
    UPARAM(Ref) EPreReplicatedActorReference& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindActorReferenceWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindActorComponentReferenceWithAccessor",
    ToolTip = "Bind a reference to any object of type UActorComponent."))
  void K2_BindActorComponentReferenceWithAccessor(
    UPARAM(ref) UActorComponent*& VariableToBind,
    UPARAM(Ref) EPreReplicatedActorComponentReference& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindActorComponentReferenceWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "BindAnimMontageReferenceWithAccessor",
    ToolTip = "Bind a reference to any object of type UAnimMontage."))
  void K2_BindAnimMontageReferenceWithAccessor(
    UPARAM(ref) UAnimMontage*& VariableToBind,
    UPARAM(Ref) EPreReplicatedAnimMontageReference& OutAccessor,
    UPARAM(DisplayName = "Replay") bool bReplicateToAutonomousProxy = true,
    UPARAM(DisplayName = "Replicate") bool bReplicateToSimulatedProxy = false,
    UPARAM(DisplayName = "Update On Change") bool bForceNetUpdateOnChange = false
  )
  {
    BindAnimMontageReferenceWithAccessor(VariableToBind, OutAccessor, bReplicateToAutonomousProxy, bReplicateToSimulatedProxy, bForceNetUpdateOnChange);
  }

  /// Blueprint-getters for pre-replicated data saved in a state. The type-specific functions return the pre-replicated variable with name
  /// "Variable" of the passed state.
  /// @attention Changing the order in which your data is bound also changes which variables refer to the bound data e.g. the first bound
  /// variable of type bool will always be bound to Bool1. To mitigate this issue data can be bound with an accessor which can then be used
  /// here to retrieve the correct values independently of the binding order.
  ///
  /// @param        State       The state that contains the data members to access.
  /// @param        Variable    The name of the variable to access.
  /// @returns      <Type>      The value of the pre-replicated member with the name "Variable".
public:

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedBoolFromState"))
  bool K2_GetPreReplicatedBoolFromState(const FState& State, EPreReplicatedBool Variable) const
  {
    return GetPreReplicatedBoolFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedHalfByteFromState"))
  uint8 K2_GetPreReplicatedHalfByteFromState(const FState& State, EPreReplicatedHalfByte Variable) const
  {
    return GetPreReplicatedHalfByteFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedByteFromState"))
  uint8 K2_GetPreReplicatedByteFromState(const FState& State, EPreReplicatedByte Variable) const
  {
    return GetPreReplicatedByteFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedIntFromState"))
  int32 K2_GetPreReplicatedIntFromState(const FState& State, EPreReplicatedInt Variable) const
  {
    return GetPreReplicatedIntFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedFloatFromState"))
  float K2_GetPreReplicatedFloatFromState(const FState& State, EPreReplicatedFloat Variable) const
  {
    return GetPreReplicatedFloatFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedVectorFromState"))
  FVector K2_GetPreReplicatedVectorFromState(const FState& State, EPreReplicatedVector Variable) const
  {
    return GetPreReplicatedVectorFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedNormalFromState"))
  FVector K2_GetPreReplicatedNormalFromState(const FState& State, EPreReplicatedNormal Variable) const
  {
    return GetPreReplicatedNormalFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedRotatorFromState"))
  FRotator K2_GetPreReplicatedRotatorFromState(const FState& State, EPreReplicatedRotator Variable) const
  {
    return GetPreReplicatedRotatorFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedActorReferenceFromState"))
  AActor* K2_GetPreReplicatedActorReferenceFromState(const FState& State, EPreReplicatedActorReference Variable) const
  {
    return GetPreReplicatedActorReferenceFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedActorComponentReferenceFromState"))
  UActorComponent* K2_GetPreReplicatedActorComponentReferenceFromState(const FState& State, EPreReplicatedActorComponentReference Variable) const
  {
    return GetPreReplicatedActorComponentReferenceFromState(State, Variable);
  }

  UFUNCTION(BlueprintCallable, Category = "General Movement Component", meta = (DisplayName = "GetPreReplicatedAnimMontageReferenceFromState"))
  UAnimMontage* K2_GetPreReplicatedAnimMontageReferenceFromState(const FState& State, EPreReplicatedAnimMontageReference Variable) const
  {
    return GetPreReplicatedAnimMontageReferenceFromState(State, Variable);
  }

protected:

  virtual void Server_SwapStateBufferBoundData();
  virtual bool Server_ForceNetUpdateCheckBoundData();
  virtual void Server_SaveBoundDataToServerState(FState& ServerState, ENetRole RecipientRole) const;
  virtual void Server_ResetLockedBoundData();
  virtual bool Client_IsBoundDataValid(const FMove& SourceMove) const;
  virtual void Client_UnpackBoundData(FState& ServerState) const;
  virtual void Client_LoadBoundDataForReplay(const FMove& SourceMove);
  virtual void SaveBoundDataToMove(FMove& Move, FMove::EStateVars VarsToSave) const;
  virtual void SaveBoundDataToState(FState& State) const;
  virtual void LoadInBoundDataFromMove(const FMove& Move) const;
  virtual void LoadBoundDataFromState(const FState& State) const;
  virtual void LoadReplicatedBoundDataFromState(const FState& State) const;
  virtual void AddTargetStateBoundDataToInitState(FState& InitializationState, const FState& TargetState) const;
  virtual void AddStartStateBoundDataToInitState(FState& InitializationState, const FState& StartState) const;
  IMPLEMENT_REPLICATION_SYSTEM()

  /// Specialized validation functions for bound data.
  bool Client_IsValidFloat(const FMove& SourceMove) const;
  bool Client_IsValidVector(const FMove& SourceMove) const;
  bool Client_IsValidNormal(const FMove& SourceMove) const;
  bool Client_IsValidRotator(const FMove& SourceMove) const;

#pragma endregion

#pragma region Default Replication Interface

  /// @attention Replication settings (for any FMove or FState) must always match between client and server for any actor instance that
  /// replicates, otherwise the (de)serialization will fail and the client will be disconnected from the server.
protected:

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Autonomous Proxy", Category = "Networking|Replication",
    ReplicatedUsing = Client_OnRepServerState_AutonomousProxy)
  FState ServerState_AutonomousProxy_Default;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Simulated Proxy", Category = "Networking|Replication",
    ReplicatedUsing = Client_OnRepServerState_SimulatedProxy)
  FState ServerState_SimulatedProxy_Default;

  FState ServerState_AutonomousProxy_Buffered_Default;
  FState ServerState_SimulatedProxy_Buffered_Default;
  FState StateBuffer_Default;
  FMove LocalMove_Default;

private:

  virtual void Client_SendMovesToServer() { Server_SendMoves(Client_PendingMoves); }
  virtual FMove& LocalMove() { return LocalMove_Default; }
  virtual const FMove& LocalMove() const { return LocalMove_Default; }
  virtual FState& ServerState_AutonomousProxy() { return ServerState_AutonomousProxy_Default; }
  virtual FState& ServerState_SimulatedProxy() { return ServerState_SimulatedProxy_Default; }
  virtual const FState& ServerState_AutonomousProxy() const { return ServerState_AutonomousProxy_Default; }
  virtual const FState& ServerState_SimulatedProxy() const { return ServerState_SimulatedProxy_Default; }
  virtual FState& ServerState_AutonomousProxy_Buffered() { return ServerState_AutonomousProxy_Buffered_Default; }
  virtual FState& ServerState_SimulatedProxy_Buffered() { return ServerState_SimulatedProxy_Buffered_Default; }
  virtual const FState& ServerState_AutonomousProxy_Buffered() const { return ServerState_AutonomousProxy_Buffered_Default; }
  virtual const FState& ServerState_SimulatedProxy_Buffered() const { return ServerState_SimulatedProxy_Buffered_Default; }

  virtual void Server_BufferServerState_AutonomousProxy()
  {
    ServerState_AutonomousProxy_Buffered_Default = ServerState_AutonomousProxy_Default;
  }

  virtual void Server_BufferServerState_SimulatedProxy()
  {
    ServerState_SimulatedProxy_Buffered_Default = ServerState_SimulatedProxy_Default;
  }

  virtual FState& StateBuffer() { return StateBuffer_Default; }

#pragma endregion

#pragma region Custom FMove Serialization Settings

  ///***************************************************************************************************************************************
  /// By default all data of FMove is serialized and sent from client to server. Since serialization options are not replicated and the data
  /// is sent via RPC argument the settings can only be changed through the C++ constructor. This section will show you how to implement
  /// different serialization settings for different child classes.
  ///***************************************************************************************************************************************

#if 0

  /// Copy and paste the code below into the header file of your child class. You can then change the serialization settings within the
  /// constructor of the derived struct as desired. Any names with the _Custom postfix are arbitrary of course.
  /// @attention Do not add any additional members to the struct, it may only be used to change the serialization options.
  USTRUCT(BlueprintType)
  struct FMove_Custom : public FMove
  {
    GENERATED_BODY()
    FMove_Custom()
    {
      InputVectorQuantize = ESizeQuantization::Short;
      OutLocationQuantize = EDecimalQuantization::RoundTwoDecimals;
      OutRotationQuantize = ESizeQuantization::Short;
      OutControlRotationQuantize = ESizeQuantization::Short;
      bSerializeInputVectorX = true;
      bSerializeInputVectorY = true;
      bSerializeInputVectorZ = true;
      bSerializeOutLocation = true;
      bSerializeOutRotationRoll = true;
      bSerializeOutRotationPitch = true;
      bSerializeOutRotationYaw = true;
      bSerializeOutControlRotationRoll = true;
      bSerializeOutControlRotationPitch = true;
      bSerializeOutControlRotationYaw = true;
      NumSerializedInputFlags = 16;
    }
    bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) { return NetSerialize_Implementation(Ar, Map, bOutSuccess); }
  };
  template<>
  struct TStructOpsTypeTraits<FMove_Custom> : public TStructOpsTypeTraitsBase2<FMove_Custom>
  {
    enum { WithNetSerializer = true };
  };

  /// Copy and paste the code below into the body of your derived class declaration to implement the functions necessary to use the struct
  /// type with your custom serialization options.
private:
  UFUNCTION(Server, Reliable, WithValidation)
  void Server_SendMoves_Custom(const TArray<FMove_Custom>& RemoteMoves);
  FMove_Custom LocalMove_Custom;
  FMove& LocalMove() override { return LocalMove_Custom; }
  const FMove& LocalMove() const override { return LocalMove_Custom; }
  void Client_SendMovesToServer() override
  {
    check(sizeof FMove_Custom == sizeof FMove)
    const TArray<FMove_Custom>& PendingMoves = reinterpret_cast<const TArray<FMove_Custom>&>(Client_GetPendingMoves());
    Server_SendMoves_Custom(PendingMoves);
  }
  void Server_SendMoves_Custom_Implementation(const TArray<FMove_Custom>& RemoteMoves)
  {
    check(sizeof FMove_Custom == sizeof FMove)
    const TArray<FMove>& ReceivedMoves = reinterpret_cast<const TArray<FMove>&>(RemoteMoves);
    Server_ProcessClientMoves(ReceivedMoves);
  }
  bool Server_SendMoves_Custom_Validate(const TArray<FMove_Custom>& RemoteMoves)
  {
    return true;
  }

  /// Alternatively you can use the shorter version below that implements most of the logic automatically with a macro.
  /// @attention The two function name comments are required, without them the UHT generates code that will prevent successful compilation.
private:
  UFUNCTION(Server, Reliable, WithValidation)
  void Server_SendMoves_Custom(const TArray<FMove_Custom>& RemoteMoves);
  /*Server_SendMoves_Custom_Implementation*/
  /*Server_SendMoves_Custom_Validate*/
  IMPLEMENT_CUSTOM_SERIALIZATION_SETTINGS(FMove_Custom, LocalMove_Custom, Server_SendMoves_Custom)

#endif

#pragma endregion
};

FORCEINLINE bool UGenMovementReplicationComponent::Rep_IsValid(const float& UnpackedValue)
{
  return !FMath::IsNaN(UnpackedValue);
}

FORCEINLINE bool UGenMovementReplicationComponent::Rep_IsValid(const FVector& UnpackedVector)
{
  return !UnpackedVector.ContainsNaN();
}

FORCEINLINE bool UGenMovementReplicationComponent::Rep_IsValid(const FRotator& UnpackedRotator)
{
  return !UnpackedRotator.ContainsNaN();
}

FORCEINLINE void UGenMovementReplicationComponent::Rep_SetInvalid(float& NonSerializedValue)
{
  NonSerializedValue = NAN;
}

FORCEINLINE void UGenMovementReplicationComponent::Rep_SetInvalid(FVector& NonSerializedVector)
{
  NonSerializedVector = FVector{NAN};
}

FORCEINLINE void UGenMovementReplicationComponent::Rep_SetInvalid(FRotator& NonSerializedRotator)
{
  NonSerializedRotator = FRotator{NAN};
}

FORCEINLINE AGenPawn* UGenMovementReplicationComponent::GetGenPawnOwner() const
{
  return GenPawnOwner;
}

FORCEINLINE bool UGenMovementReplicationComponent::IsReplaying() const
{
  return Client_bIsReplaying;
}

FORCEINLINE bool UGenMovementReplicationComponent::IsExecutingRemoteMoves() const
{
  return Server_bIsExecutingRemoteMoves;
}

FORCEINLINE bool UGenMovementReplicationComponent::IsExtrapolating() const
{
  // @see bUsingExtrapolatedData can be true even when @see bAllowExtrapolation is false.
  return bAllowExtrapolation && bUsingExtrapolatedData;
}

FORCEINLINE const FState& UGenMovementReplicationComponent::GetLastInterpolationState() const
{
  return bUsingExtrapolatedData ? ExtrapolatedState : InterpolatedState;
}

FORCEINLINE const FState& UGenMovementReplicationComponent::GetLastInterpolationStartState() const
{
  return InterpolationStartState;
}

FORCEINLINE const FState& UGenMovementReplicationComponent::GetLastInterpolationTargetState() const
{
  return InterpolationTargetState;
}

FORCEINLINE int32 UGenMovementReplicationComponent::GetCurrentInterpolationStartStateIndex() const
{
  return CurrentStartStateIndex;
}

FORCEINLINE int32 UGenMovementReplicationComponent::GetCurrentInterpolationTargetStateIndex() const
{
  return CurrentTargetStateIndex;
}

FORCEINLINE bool UGenMovementReplicationComponent::IsInterpolationDataCurrent() const
{
  return CurrentStartStateIndex >= 0 && CurrentTargetStateIndex >= 0;
}

FORCEINLINE const FState& UGenMovementReplicationComponent::AccessStateQueue(int32 Index) const
{
  return StateQueue[Index];
}

FORCEINLINE bool UGenMovementReplicationComponent::IsValidStateQueueIndex(int32 Index) const
{
  return Index >= 0 && Index < StateQueue.Num();
}

FORCEINLINE int32 UGenMovementReplicationComponent::GetStateQueueSize() const
{
  return StateQueue.Num();
}

FORCEINLINE int32 UGenMovementReplicationComponent::GetStateQueueMaxSize() const
{
  return StateQueueMaxSize;
}

FORCEINLINE FVector UGenMovementReplicationComponent::GetVelocity() const
{
  return Velocity;
}

FORCEINLINE void UGenMovementReplicationComponent::SetVelocity(const FVector& NewVelocity)
{
  Velocity = NewVelocity;
  UpdateComponentVelocity();
}

FORCEINLINE const TArray<FMove>& UGenMovementReplicationComponent::Client_GetPendingMoves() const
{
  return Client_PendingMoves;
}
