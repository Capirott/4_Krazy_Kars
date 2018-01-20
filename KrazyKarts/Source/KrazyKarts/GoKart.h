// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;
	
	UPROPERTY()
	float Time;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FGoKartMove LastMove;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTransform Transform;

};

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	float Mass = 1000.0f;

	UPROPERTY(EditAnywhere)
	float MinimumTurningRadius = 10.0f;

	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;
	
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);	
	
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;
	
	FVector Velocity;

	void MoveRight(float value);
	void MoveForward(float value);

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
	FVector GetAirResistance();
	FVector GetRollingResistance();

	UFUNCTION()
	void OnRep_ServerState();
	
	UPROPERTY(Replicated)
	float Throttle;
	UPROPERTY(Replicated)
	float SteeringThrow;
	
};
