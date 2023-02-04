// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoeaTechEval.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

/**
 * 
 */
class AGoeaTechEvalCharacter;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Climb			UMETA(DisplayName = "Climb"),
	CMOVE_MAX			UMETA(Hidden),
};

UCLASS()
class GOEATECHEVAL_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_Custom : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;
		enum CompressedFlags
		{
			FLAG_Climb = 0x10,
		};

		uint8 Saved_ToClimb : 1;


		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	bool Safe_ToClimb;

	class FNetworkPredictionData_Client_Custom : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Custom(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	UPROPERTY(EditDefaultsOnly) float ClimeSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly) float WallRunPullAwayAngle = 75;
	UPROPERTY(EditDefaultsOnly) float WallAttractionForce = 200.f;
	UPROPERTY(EditDefaultsOnly) float MinWallRunHeight = 50.f;
	UPROPERTY(EditDefaultsOnly) UCurveFloat* WallRunGravityScaleCurve;
	UPROPERTY(EditDefaultsOnly) float WallJumpOffForce = 300.f;

	UPROPERTY(EditDefaultsOnly) AGoeaTechEvalCharacter* GoeaCharacterOwner;

	
	public:
		UCustomMovementComponent();
		float CapR() const;

		virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
		virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

		virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
		virtual bool CanAttemptJump() const override;
		virtual bool DoJump(bool bReplayingMoves) override;


		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) UCustomMovementComponent* CustomMovementComponent;
		UFUNCTION(BlueprintPure) FORCEINLINE UCustomMovementComponent* GetCostomCharacterMovement() const { return CustomMovementComponent; }


		UFUNCTION(BlueprintCallable) void Enable_Climb();
		UFUNCTION(BlueprintCallable) void Disable_Climb();
		UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
		UFUNCTION(BlueprintPure) bool IsCliming() const { return IsCustomMovementMode(CMOVE_Climb); }
		


	protected:
		virtual void UpdateFromCompressedFlags(uint8 Flags) override;
		virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
		virtual void PhysCustom(float deltaTime, int32 Iterations) override;
		virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
		virtual void InitializeComponent() override;
		

		UPROPERTY(BlueprintReadWrite) bool Onclimb = false;
		UPROPERTY(BlueprintReadWrite) float Climb_speed = 300.f;

	private:

		
		void EnterClimb(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
		void ExitClimb();
		bool Tryclimb();
		double AngleBetween(FVector a, FVector b);
		bool CanClimb() const;
		virtual void MoveAlongWall(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult = NULL);
		void PhysClimb(float deltaTime, int32 Iterations);

		
};
