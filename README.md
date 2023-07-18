
#by Chenghao Li

This project focuses on the Climbing system and the part of the Combo System. 




## Controls
The player can move with WASD.

Press space to jump in the open area, climbing when facing the wall. Press space again jumps off the wall during the climbing.

Left click to light attack. Continue clicking at the right time for a combo.

"Q" for Iaido, hold "Q" for charge. Continue pressing at the right time for a combo

"E" for dodge attack. Continue pressing at the right time for a combo





## Rules
The level contains a complicated structure to climb around, and two destructible actors to play around.


## Outside assets

https://tp9wnq.am.files.1drv.com/y4mRD2wmzL20Z23cXWBVxizJ9q3u4HYOEewGd9ko3KhCIBzHfgNAOWfsrJ_3nJDgfeh0lvokRD12RvDgl8vFCgLIZuxIwMSeJMn96FtBIfj9NZvR2TOAz37DjWIeS7eT7bhlKwRZOJMChxYPCU9uqKD8DRBoaNdv15s_86jjawq90KdS7v_e8R9V8caIix-RpOQurG_Mv-o_7V893rsxxgd6Q

Climb up animation

## Documentation

Attack implementation/Combo 

I create Blueprint Macros to stop move input and set Jump Velocity.Z to 0, so the player can not move while attacking.
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Disable_move.png?raw=true)
Enable is reverse it.
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Enable_move.png?raw=true)

The player should press the same key at a critical time to trigger the combo. By using Attack_c Macros, the game designer can easily and fast add new combos attacks with editable critical time. 

How to use Attack_c:
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_aim.png?raw=true)
This is a functional animation montage. It contains 4 animations, grouped as Attack1-4. Also, group Attack1-2, 2-3, 3-4 represent the connection time between two combo animations. The time between Notifies Attack 1-2,2-3,3-4 and Group Attack 1-2,2-3,3-4. Represent the time can take input to trigger the next combo attack.

![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_aim2.png?raw=true)
A game designer should line up Attack1-4 and leave the connection time group alone. 

![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Use%20Attack_c.png?raw=true)
A game designer only needs to pass the montage name and an array of Notifies to trigger the next combo

How Attack_c work:
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_c1.png?raw=true)
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_c2.png?raw=true)
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_c3.png?raw=true)
Attack_c takes the input of the montage name and an array of combo Notifies. The montage will play the entire combo animation, but when time goes to combo Notifies, if the player does not press the attack key again, the entire combo animation will stop and play the Attack1-2 animation, which is the same name as combo Notifies. Overall, break the montage and play the rest single attack montage if the player did not press the attack key. 

Current issue:
The combo system still needs to add more check variables about how to perform different attack modes, such as charge attack, I managed to make a charge attack with brutal force, but when character charging, the input is still disabled. 


=======================================================================================================================

Climbing:

Critical functions

Tryclimb()
This function call when the player wants||can climb, the function call the UpdateAverageHit() function to check if the wall front player climable, if it is, set the average normal. 
```C++

CustomMovementComponent.h
// Detection climb
bool Tryclimb();

CustomMovementComponent.cpp
bool UCustomMovementComponent::Tryclimb()
{
	if (!bWantsToClimb) {
		
		return false;
	}

	if (UpdateAverageHit())
	{
		
		Safe_ToClimb = true;
	}
	else {
		Safe_ToClimb = false;

		return false;
	}
	
	SetMovementMode(MOVE_Custom, CMOVE_Climb);
	
	return true;
}

```
UpdateAverageHit()

Shoot 5 lines to detect the wall front player wall climbing and trying climbing. Return false if there no hit, return true if it is hit. Also, average the normal of 5 lines and set averagehit.normal to it. With the average normal, we can handle the complicacy surface.

![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Trace.png?raw=true)
```C++
CustomMovementComponent.h

	// Average hit information by 5 different hit point
    FHitResult Average_WallHit;
	bool UpdateAverageHit();

CustomMovementComponent.cpp
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
```

PhysClimb()
Active new PhysClimb when try climbing return true. If the player reaches to the top, play root animation to move the top and stop climbing. If player hit space to change bWandtoclimb, stop climbing. if player hit the ground, stop climbing. other while, set move forward&backward to move left and right.

set Move_left_right and Move_up_down to get speed to control climbing animation
```C++
if (CustomCharacterMovementComponent->IsCliming())
		{
			Direction = FVector::CrossProduct(CustomCharacterMovementComponent->Average_WallHit.Normal, -GetActorRightVector());
		}
```

```C++
CustomMovementComponent.cpp
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
			

			FHitResult Hit(1.f);
			AnimInstance->Montage_Play(Hang_to_Crouch_Montage);
			bWantsToClimb = true;
			Velocity = FVector(Velocity.X, Velocity.Y, 50);
			const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
			//UpdatedComponent->SetRelativeRotation(UpdatedComponent->GetComponentRotation());
			SafeMoveUpdatedComponent(Velocity, UpdatedComponent->GetComponentRotation(), true, Hit);
			SetMovementMode(EMovementMode::MOVE_Walking);
			
			return;
		}

		RestorePreAdditiveRootMotionVelocity();

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			
			constexpr bool bFluid = false;
			CalcVelocity(deltaTime, Friction, bFluid, 900.f);
		}

		ApplyRootMotionToVelocity(deltaTime);

		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	
		const FVector Adjusted = Velocity * deltaTime;

		FHitResult Hit(1.f);
```
using CurrentClimbingNormal to set player rotation during climbing, SlideAlongSurface() can use for handle sliding resistance，and set Friction to simulate different kind of surfaces, such as ice walls.


```C++
CustomMovementComponent.h
float Friction = 10;
CustomMovementComponent.cpp
		//climbingrotation:
		const FQuat Current = UpdatedComponent->GetComponentQuat();
		const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();
		FQuat Climbingrotation = FMath::QInterpTo(Current, Target, deltaTime, 6);

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
```

Current issues/bugs:
I believe I address the focus of the feature, the climbing is working and it can handle a lot edge case. The biggest bug is when climbing up, the root animation will always turn to the x-axes, I believe it is because the root animation has some issue, so the Transition between climbing and climbing up is not that intuitive. 








