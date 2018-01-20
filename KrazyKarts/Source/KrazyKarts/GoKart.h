// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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

	FVector Velocity;
	void MoveForward(float value);

	void MoveRight(float value);
	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
	FVector GetAirResistance();
	FVector GetRollingResistance();
	
	float Throttle;
	float SteeringThrow;
	
};
