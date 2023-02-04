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
	if (IsFalling())
	{
		Tryclimb();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
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
	bool bIsCliming = IsCliming();
	if (Super::DoJump(bReplayingMoves))
	{
		if (bIsCliming)
		{
			FVector Start = UpdatedComponent->GetComponentLocation();
			
			auto Params = GoeaCharacterOwner->GetIgnoreCharacterParams();
			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(WallHit, Start, WallHit.Normal * 2, "BlockAll", Params);
			Velocity += WallHit.Normal * WallJumpOffForce;
			Velocity.Z += 300.f;
		}

		return true;
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
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
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

void UCustomMovementComponent::EnterClimb(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	FVector Start = UpdatedComponent->GetComponentLocation();
}

void UCustomMovementComponent::ExitClimb()
{

}

bool UCustomMovementComponent::Tryclimb()
{
	
	


	if (!IsFalling())
	{
		return false;
	}
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + UpdatedComponent->GetForwardVector()*70;

	FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * 100;
	FVector RightEnd = Start + UpdatedComponent->GetRightVector() * 100;

	//Start += Start + UpdatedComponent->GetForwardVector() * 50;
	FCollisionQueryParams Params = GoeaCharacterOwner->GetIgnoreCharacterParams();
	
	//Params.AddIgnoredActor(AGoeaTechEvalCharacter);
	

	FHitResult WallHit, FloorHit;

	GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	if (GEngine && WallHit.GetActor())
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString::Printf(TEXT(" %s"), *WallHit.GetActor()->GetName()));
	if (WallHit.IsValidBlockingHit())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("2"));
		Safe_ToClimb = true;
	}
	else return false;

	SetMovementMode(MOVE_Custom, CMOVE_Climb);
	
	return true;
}

double UCustomMovementComponent::AngleBetween(FVector a, FVector b)
{
	//θ = cos-1( (a · b) / (|a| * |b|) )
	//θ=acos(v1⋅v2/||v1||||v2||)
	double angle = 0.0f;
	
	
	b.Normalize();
	a.Normalize();
		
	
	
	angle = UKismetMathLibrary::Acos(FVector::DotProduct(a, b));
	//angle = UKismetMathLibrary::Acos(FVector::DotProduct(a, b) );

	//DrawDebugLine(GetWorld(), a,
	//	a + Start,
	//		FColor(255, 0, 0),
	//		false, 0.1, 0,
	//		5
	//	);
	

	angle = FMath::RadiansToDegrees(angle);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("angle %f"), angle));
	
	return angle;
}

bool UCustomMovementComponent::CanClimb() const
{
	return false;
}

void UCustomMovementComponent::MoveAlongWall(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult)
{
}

void UCustomMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = deltaTime;
	//bOrientRotationToMovement = false;

	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector CastDelta = UpdatedComponent->GetRightVector() * 100;
		FVector End = Safe_ToClimb ? Start + CastDelta : Start - CastDelta;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		auto Params = GoeaCharacterOwner->GetIgnoreCharacterParams();
		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
		bool isUp = false;
		


		


		
			

		FVector tmp = Velocity;
		float angle_bt = 999;
		Velocity.Normalize();
		FVector Nhit, BNhit;
		FRotator Wallrotation = FRotator(0,0,0);
		FHitResult WallHit, BackHit;
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, Acceleration + Start, "BlockAll", Params);
		if (WallHit.IsValidBlockingHit()) {
			Nhit = WallHit.Normal;
			
			Wallrotation = UKismetMathLibrary::MakeRotFromX(Nhit);
		
			angle_bt = AngleBetween(WallHit.Location + (WallHit.Normal * 70), Acceleration + Start);
			isUp = true;
		}
		else {
			GetWorld()->LineTraceSingleByProfile(BackHit, Start, -Acceleration + Start, "BlockAll", Params);
			if (BackHit.IsValidBlockingHit()) {

				BNhit = BackHit.Normal;
				Wallrotation = UKismetMathLibrary::MakeRotFromX(BNhit);
				angle_bt = AngleBetween(BackHit.Location + (BackHit.Normal * 70), Acceleration + Start);
				isUp = false;
			}
		}



		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("angle: %f"), angle_bt));


		CalcVelocity(timeTick, 0.f, false, 1.f);
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		
		
		//accelerate
		FVector Vtmp, Vtmp2;
		Acceleration;
		//GoeaCharacterOwner->enhandle;
		if (angle_bt < 60) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString::Printf(TEXT("up")));
		
			Vtmp = FVector::CrossProduct(Nhit, Acceleration);
			Vtmp.Normalize();
			Vtmp = Vtmp * 200;
			Vtmp.Z = abs(Vtmp.Z);


			//Vtmp = Vtmp.ProjectOnTo(Nhit);
			Vtmp2 = (Vtmp + WallHit.Location) - Start;

			Vtmp2 = FVector::VectorPlaneProject(Vtmp2,Nhit);
			//Vtmp2 += Start;
			Vtmp2.Normalize();
			Vtmp2 = Vtmp2 * 50;
			//Vtmp2 = Vtmp2.ProjectOnTo(Nhit);
			//Vtmp2 = Vtmp2 + Nhit * 70;
				DrawDebugLine(GetWorld(), Start,
					Vtmp2*50 + Start,
					FColor(255, 0, 0),
					false, 0.1, 0,
					5
				);

			
			Acceleration = Vtmp2;
			Velocity = Vtmp2 * 10;
			
	
		}
		else if (!isUp) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString::Printf(TEXT("down")));

			Vtmp = FVector::CrossProduct(BNhit, Acceleration);
			Vtmp.Normalize();
			Vtmp = Vtmp * 200;
			Vtmp.Z = abs(Vtmp.Z);


			//Vtmp = Vtmp.ProjectOnTo(Nhit);
			Vtmp2 = (Vtmp + BackHit.Location) - Start;

			Vtmp2 = FVector::VectorPlaneProject(Vtmp2, BNhit);
			//Vtmp2 += Start;
			Vtmp2.Normalize();
			Vtmp2 = Vtmp2 * 50;
			//Vtmp2 = Vtmp2.ProjectOnTo(Nhit);
			//Vtmp2 = Vtmp2 + Nhit * 70;


			//Acceleration = Vtmp2;
			//Velocity = Vtmp2 * 10;

			//Velocity = Vtmp2 * 10

		}
		

		


		const FVector Delta = timeTick * Velocity; // dx = v * dt
		const bool bZeroDelta = Delta.IsNearlyZero();
		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			
			FHitResult Hit;
			if (isUp) {
				SafeMoveUpdatedComponent(Delta,
					FRotator(UpdatedComponent->GetComponentRotation().Pitch, Wallrotation.Yaw + 180, UpdatedComponent->GetComponentRotation().Roll), true, Hit);
			}
			else {
				SafeMoveUpdatedComponent(Delta,
					FRotator(UpdatedComponent->GetComponentRotation().Pitch, Wallrotation.Yaw + 180, UpdatedComponent->GetComponentRotation().Roll), true, Hit);
			}
			
		}
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick; // v = dx / dt

	}

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
	}
	bOrientRotationToMovement = true;
}

void UCustomMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	GoeaCharacterOwner = Cast<AGoeaTechEvalCharacter>(GetOwner());
}

