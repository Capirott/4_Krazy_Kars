#include "GoKartMovementReplicator.h"

#include "Net/UnrealNetwork.h"


UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}


void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& LastMove)
{
	ServerState.LastMove = LastMove;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (MovementComponent == nullptr) return;

	FGoKartMove LastMove = MovementComponent->GetLastMove();
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnacknowledgeMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		UpdateServerState(LastMove);
	}
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}

void UGoKartMovementReplicator::ClearAcknowledgeMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : UnacknowledgeMoves)
	{
		if (LastMove.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}
	UnacknowledgeMoves = NewMoves;
}

void UGoKartMovementReplicator::OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);
	ClearAcknowledgeMoves(ServerState.LastMove);
	for (const FGoKartMove& Move : UnacknowledgeMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove LastMove)
{
	if (MovementComponent == nullptr) return;
	MovementComponent->SimulateMove(LastMove);
	UpdateServerState(LastMove);
}

bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	return true;
}

