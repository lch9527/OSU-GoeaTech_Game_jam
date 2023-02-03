// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMovementComponent.h"
#include "GoeaTechEvalCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"



bool UCustomMovementComponent::FSavedMove_Custom::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_Custom* NewCustomMove = static_cast<FSavedMove_Custom*>(NewMove.Get());

	if (Saved_ToClimb != NewCustomMove->Saved_ToClimb)
	{
		
		return false;
	}


	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UCustomMovementComponent::FSavedMove_Custom::Clear()
{
	FSavedMove_Character::Clear();
	Saved_ToClimb = 0;
}

uint8 UCustomMovementComponent::FSavedMove_Custom::GetCompressedFlags() const
{

	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (Saved_ToClimb) Result |= FLAG_Climb;

	return Result;
}

void UCustomMovementComponent::FSavedMove_Custom::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const UCustomMovementComponent* CharacterMovement = Cast<UCustomMovementComponent>(C->GetCharacterMovement());


	Saved_ToClimb = CharacterMovement->Safe_ToClimb;

}

void UCustomMovementComponent::FSavedMove_Custom::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UCustomMovementComponent* CharacterMovement = Cast<UCustomMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_ToClimb = Saved_ToClimb;

}

UCustomMovementComponent::FNetworkPredictionData_Client_Custom::FNetworkPredictionData_Client_Custom(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{

}
/*
UZippyCharacterMovementComponent::FNetworkPredictionData_Client_Zippy::FNetworkPredictionData_Client_Zippy(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{
}
*/

FSavedMovePtr UCustomMovementComponent::FNetworkPredictionData_Client_Custom::AllocateNewMove()
{
	//FSavedMove_Custom
	return FSavedMovePtr(new FSavedMove_Custom());
}

UCustomMovementComponent::UCustomMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

FNetworkPredictionData_Client* UCustomMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

		if (ClientPredictionData == nullptr)
		{
			UCustomMovementComponent* MutableThis = const_cast<UCustomMovementComponent*>(this);

			MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Custom(*this);
			MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
			MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
		}
	return ClientPredictionData;

}

void UCustomMovementComponent::Enable_Climb()
{
}

void UCustomMovementComponent::Disable_Climb()
{
}

void UCustomMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_ToClimb = (Flags & FSavedMove_Custom::FLAG_Custom_0) != 0;
	

}

void UCustomMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	//Safe_bPrevWantsToCrouch = bWantsToCrouch;
}
