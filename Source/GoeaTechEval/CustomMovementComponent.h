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
	// Standard set up from UCharacterMovementComponent
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
	UPROPERTY(EditDefaultsOnly) AGoeaTechEvalCharacter* GoeaCharacterOwner;

	
	public:
		UCustomMovementComponent();
		// Climbing vriable 
		float Friction = 10;
		UPROPERTY(BlueprintReadOnly) float Move_left_right = 0.f;
		UPROPERTY(BlueprintReadOnly) float Move_up_down = 0.f;
		UFUNCTION(BlueprintCallable) void Set_Friction(float F) { Friction = F; }

		// Override state update
		virtual void UpdateFromCompressedFlags(uint8 Flags) override;
		virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
		virtual void InitializeComponent() override;
		virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
		virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
		virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
		virtual bool CanAttemptJump() const override;
		virtual bool DoJump(bool bReplayingMoves) override;

		// Detection climb
		bool Tryclimb();
		// Average hit information by 5 different hit point
		FHitResult Average_WallHit;
		bool UpdateAverageHit();
		

		// Enable Ik
		UFUNCTION(BlueprintPure) bool Is_Stop() const { return Velocity.Size() <= 10 ? true : false; }
		// Cheak MovementMode
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) UCustomMovementComponent* CustomMovementComponent;
		UFUNCTION(BlueprintPure) FORCEINLINE UCustomMovementComponent* GetCostomCharacterMovement() const { return CustomMovementComponent; }
		UFUNCTION(BlueprintPure) bool IsCliming() const { return IsCustomMovementMode(CMOVE_Climb); }
		UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
		UPROPERTY(Category = "Climbing", EditDefaultsOnly)
			UAnimMontage* Hang_to_Crouch_Montage;
		UPROPERTY()
			UAnimInstance* AnimInstance;
		


	protected:
		// Situation check virable 
		FVector Up_location;
		bool bWantsToClimb = false;
		FVector CurrentClimbingNormal;
		FVector CurrentClimbingPosition;
		// Situation check function
		bool ValidToFloor();
		bool ValidToTop();


		
		// override PhysCustom
		void PhysClimb(float deltaTime, int32 Iterations);
		virtual void PhysCustom(float deltaTime, int32 Iterations) override;
		void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	

	private:

		virtual void BeginPlay() override;
		//Math help function
		double AngleBetween(FVector a, FVector b);


		
};
