// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Error";
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceletation = Force / Mass;

	Velocity += Acceletation * DeltaTime;
	ApplyRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);
	if (HasAuthority())
	{
		ReplicatedLocation = GetActorLocation();
		ReplicatedRotation = GetActorRotation();
	}
	else
	{
		SetActorRotation(ReplicatedRotation);
		SetActorLocation(ReplicatedLocation);
	}
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);
}

FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AGoKart::GetRollingResistance()
{
	float AcceletationDueToGravity = -GetWorld()->GetGravityZ() / 100.0f;
	float NormalForce = Mass * AcceletationDueToGravity;
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void AGoKart::ApplyRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinimumTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);
	Velocity = RotationDelta.RotateVector(Velocity);
	this->AddActorWorldRotation(RotationDelta);
}

void AGoKart::MoveRight(float value)
{
	SteeringThrow = value;
	Server_MoveRight(value);
}

void AGoKart::MoveForward(float value)
{
	Throttle = value;
	Server_MoveForward(value);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * DeltaTime * 100;
	FHitResult HitResult;
	AddActorWorldOffset(Translation, true, &HitResult);
	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value) <= 1.0f;
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1.0f;
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ReplicatedLocation);
}