// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class GOEATECHEVAL_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_Custom : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;
		/*enum CompressedFlags
		{
			FLAG_Sprint = 0x10,
			FLAG_Dash = 0x20,
			FLAG_Custom_2 = 0x40,
			FLAG_Custom_3 = 0x80,
		};*/

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
	
	public:
		UCustomMovementComponent();

		virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;


		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) UCustomMovementComponent* CustomMovementComponent;
		UFUNCTION(BlueprintPure) FORCEINLINE UCustomMovementComponent* GetCostomCharacterMovement() const { return CustomMovementComponent; }


		UFUNCTION(BlueprintCallable) void Enable_Climb();
		UFUNCTION(BlueprintCallable) void Disable_Climb();


	protected:
		virtual void UpdateFromCompressedFlags(uint8 Flags) override;
		virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

		UPROPERTY(BlueprintReadWrite) bool Onclimb = false;
		UPROPERTY(BlueprintReadWrite) float Climb_speed = 300.f;

	
};
