// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

void UCustomMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	/*GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("1111"));*/
	if (IsFalling()) {
		bWantsToClimb = true;
		Tryclimb();
	}
	
	
}

void UCustomMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
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

bool UCustomMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsCliming();
}

bool UCustomMovementComponent::DoJump(bool bReplayingMoves)
{
	
	if (IsCliming())
	{
		FVector Start = UpdatedComponent->GetComponentLocation();
		auto Params = GoeaCharacterOwner->GetIgnoreCharacterParams();
		FHitResult Hit;
		GetWorld()->LineTraceSingleByProfile(Hit, Start, Hit.Normal * 2, "BlockAll", Params);
		Velocity += Hit.Normal * WallJumpOffForce;
		Velocity.Z += 300.f;
		bWantsToClimb = false;
		SetMovementMode(EMovementMode::MOVE_Falling);
		
		
	}
	else {
		bWantsToClimb = true;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("TryClimb"));
		if (!Tryclimb()) {
			if (Super::DoJump(bReplayingMoves))
			{
				return true;
			}
			
		}
	}
	
	return false;
}
float UCustomMovementComponent::CapR() const
{
	return GoeaCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

void UCustomMovementComponent::Enable_Climb()
{
}

void UCustomMovementComponent::Disable_Climb()
{
}

bool UCustomMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return EMovementMode::MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}



void UCustomMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_ToClimb = (Flags & FSavedMove_Custom::FLAG_Custom_0) != 0;
	

}

//void UCustomMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
//{
//	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
//
//}

void UCustomMovementComponent::EnterClimb(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	FVector Start = UpdatedComponent->GetComponentLocation();
}

void UCustomMovementComponent::ExitClimb()
{

}

bool UCustomMovementComponent::Tryclimb()
{
	if (!bWantsToClimb) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("wantsFalse"));
		return false;
	}
	UpdateAverageHit();

	if (UpdateAverageHit())
	{
		
		Safe_ToClimb = true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Hitfalse"));
		Safe_ToClimb = false;

		return false;
	}
	

	SetMovementMode(MOVE_Custom, CMOVE_Climb);
	
	return true;
}


double UCustomMovementComponent::AngleBetween(FVector a, FVector b)
{
	//θ = cos-1( (a · b) / (|a| * |b|) )
	double angle = 0.0f;
	
	b.Normalize();
	a.Normalize();

	angle = UKismetMathLibrary::Acos(FVector::DotProduct(a, b));

	angle = FMath::RadiansToDegrees(angle);
	
	return angle;
}


bool UCustomMovementComponent::UpdateAverageHit()
{

	FHitResult THit, THit2, THit3, THit4, THit5;
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = UpdatedComponent->GetForwardVector();
	FVector right = UpdatedComponent->GetRightVector();
	FVector left = -right;
	bool ValidToClim = false;

	FCollisionQueryParams Params = GoeaCharacterOwner->GetIgnoreCharacterParams();

	FVector h_start = FVector(Start.X, Start.Y, Start.Z + 50);
	FVector h_end = End * 100 + FVector(Start.X, Start.Y, Start.Z + 80);
	FVector l_start = (left * -20 + End) + FVector(Start.X, Start.Y, Start.Z + 10);
	FVector l_end = (left * -50 + End * 90) + FVector(Start.X, Start.Y, Start.Z);
	FVector r_start = (right * -20 + End) + FVector(Start.X, Start.Y, Start.Z + 10);
	FVector r_end = (right * -50 + End * 90) + FVector(Start.X, Start.Y, Start.Z);


	/*DrawDebugLine(GetWorld(), Start, End * 100 + FVector(Start.X, Start.Y, Start.Z - 50), FColor::Black, false, 0.1, 0, 5);
	DrawDebugLine(GetWorld(), Start, h_end, FColor::Green, false, 0.1, 0, 5);
	DrawDebugLine(GetWorld(), Start, l_end, FColor::Blue, false, 0.1, 0, 5);
	DrawDebugLine(GetWorld(), Start, r_end , FColor::Red, false, 0.1, 0, 5);
	DrawDebugLine(GetWorld(), Start, End * 90 + Start, FColor::Purple, false, 0.1, 0, 5);*/

	GetWorld()->LineTraceSingleByProfile(THit, Start, End * 90 + Start, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(THit2, Start, h_end, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(THit3, Start, l_end, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(THit4, Start, r_end, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(THit5, Start, End * 80 + FVector(Start.X, Start.Y, Start.Z - 50), "BlockAll", Params);

	TArray<FHitResult> Hittmp = { THit , THit2 , THit3, THit4, THit5 };
	int size = 0;
	Average_WallHit = THit;

	for (int i = 0; i < 4; i++) {
		if (Hittmp[i].IsValidBlockingHit()) {
			size += 1;
			ValidToClim = true;
			Average_WallHit.ImpactPoint = Hittmp[i].ImpactPoint;
		}
	}

	Average_WallHit.Normal = (THit.Normal + THit2.Normal + THit3.Normal + THit4.Normal + THit5.Normal) / size;
	
	return ValidToClim;
}



bool UCustomMovementComponent::ValidToFloor()
{
	if (IsCliming() || bWantsToClimb) {
		return false;
	}
	FHitResult FHit;
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = -UpdatedComponent->GetUpVector();
	FRotator Rt = UpdatedComponent->GetComponentRotation();
	auto Params = GoeaCharacterOwner->GetIgnoreCharacterParams();
	Rt.Pitch = -Rt.Pitch;
	Rt.Yaw = -Rt.Yaw;
	Rt.Roll = -Rt.Roll;

	return GetWorld()->LineTraceSingleByProfile(FHit, FVector(Start.X, Start.Y, Start.Z - 120), End * 50 + Start, "BlockAll", Params);

}

bool UCustomMovementComponent::ValidToTop()
{
	FHitResult THit, THit2;
	FVector Self = UpdatedComponent->GetComponentLocation();
	FVector Up = UpdatedComponent->GetUpVector();
	FVector Frount = UpdatedComponent->GetForwardVector();

	FVector End = Up * 50 + Frount * 50 + Self;
	FVector Start = Up * 100 + Frount * 50 + Self;

	DrawDebugLine(GetWorld(), Start, End, FColor::Purple, false, 0.1, 0, 5);

	DrawDebugLine(GetWorld(), Up * 80 + Self, Up * 80 + Frount * 50 + Self, FColor::Purple, false, 0.1, 0, 5);

	auto Params = GoeaCharacterOwner->GetIgnoreCharacterParams();

	if (!GetWorld()->LineTraceSingleByProfile(THit2, Up * 80 + Self, Up * 80 + Frount * 50 + Self, "BlockAll", Params))
	{
		return GetWorld()->LineTraceSingleByProfile(THit, Start, End, "BlockAll", Params);
	}


	return false;
}




void UCustomMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{

	CurrentClimbingNormal = FVector(0,0,0);
	CurrentClimbingPosition = FVector(0, 0, 0);
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	
	bJustTeleported = false;
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = UpdatedComponent->GetForwardVector();
	auto Params = GoeaCharacterOwner->GetIgnoreCharacterParams();
	float remainingTime = deltaTime;

		UpdateAverageHit();
		CurrentClimbingNormal = Average_WallHit.Normal;
		CurrentClimbingPosition = Average_WallHit.ImpactPoint;


		

		Move_left_right = UKismetMathLibrary::LessLess_VectorRotator(Velocity, UpdatedComponent->GetComponentRotation()).Y;
		Move_up_down = UKismetMathLibrary::LessLess_VectorRotator(Velocity, UpdatedComponent->GetComponentRotation()).Z;



		
		
		if (!bWantsToClimb || CurrentClimbingNormal.IsZero())
		{

			bWantsToClimb = false;
			SetMovementMode(EMovementMode::MOVE_Falling);
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Velocity, FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0), true, Hit);
			return;
		}
		if (ValidToFloor()) {
			bWantsToClimb = false;
			SetMovementMode(EMovementMode::MOVE_Walking);
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Velocity, FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0), true, Hit);
			return;
		}
		if (ValidToTop()) {
			bWantsToClimb = false;

			
			AnimInstance->Montage_Play(Hang_to_Crouch_Montage);
			const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
			UpdatedComponent->SetRelativeRotation(UpdatedComponent->GetComponentRotation());
			//SetMovementMode(EMovementMode::MOVE_Walking);
			
			return;
		}

		RestorePreAdditiveRootMotionVelocity();

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			constexpr float Friction = 10.0f;
			constexpr bool bFluid = false;
			CalcVelocity(deltaTime, Friction, bFluid, 900.f);
		}

		ApplyRootMotionToVelocity(deltaTime);

		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	
		const FVector Adjusted = Velocity * deltaTime;

		FHitResult Hit(1.f);

		//climbingrotation:
		const FQuat Current = UpdatedComponent->GetComponentQuat();
		const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();
		FQuat Climbingrotation = FMath::QInterpTo(Current, Target, deltaTime, 4);

		SafeMoveUpdatedComponent(Adjusted, Climbingrotation, true, Hit);

		if (Hit.Time < 1.f)
		{
			HandleImpact(Hit, deltaTime, Adjusted);
	
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
		}

		const FVector Forward = UpdatedComponent->GetForwardVector();
		const FVector Location = UpdatedComponent->GetComponentLocation();
		const FQuat Rotation = UpdatedComponent->GetComponentQuat();

		const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
		const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length());

		constexpr bool bSweep = true;
		UpdatedComponent->MoveComponent(Offset * deltaTime * 2, Rotation, bSweep);

}

void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
}
	

void UCustomMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Climb:
		PhysClimb(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

void UCustomMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (IsCliming()) {
		bOrientRotationToMovement = false;
		bWantsToClimb = true;
	}
	else {
		FHitResult Hit(1.f);
		
		SafeMoveUpdatedComponent(FVector(0,0,0), FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0), true, Hit);
		//bWantsToClimb = false;
		bOrientRotationToMovement = true;
	}
	
}

void UCustomMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	GoeaCharacterOwner = Cast<AGoeaTechEvalCharacter>(GetOwner());
	
}

