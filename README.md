
# GoeaTechEval by Chenghao Li

This project focus on Climbing system, and the part of the Combo System. 




## Controls
The player can move with WASD.

Press space to jump in the open area, climbing when facing the wall. Press space again jump off the wall during the climbing.

Left click to to light attack. Continue click at right time for combo.

"Q" for Iaido, hold "Q" for charge. Continue press at right time for combo

"E" for dodge attack. Continue press at right time for combo





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
A game designer only needs to passing the montage name and an array of Notifies to trigger next combo

How Attack_c work:
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_c1.png?raw=true)
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_c2.png?raw=true)
![ScreenShot](https://github.com/lch9527/immage-holder/blob/main/Attack_c3.png?raw=true)
Attack_c takes the input of montage name and an array of combo Notifies. The montage will play the entire combo animation, but when time goes to combo Notifies, if the player does not press the attack key again, the entire combo animation will stop and play the Attack1-2 animation, which is the same name as combo Notifies. Overall, break the montage and play the rest single attack montage if the player did not press the attack key. 

Current issue:
The combo system still needs to add more check variables about how to perform different attack modes, such as charge attack, I managed to make a charge attack with brutal force, but when character charging, the input is still disabled. If I have more time , I will polish this system, make it easy to use and handle more attack modes, such as adding more variables about speed to trigger a dash attack, and also rewrite this function as c++ to make it faster. There have so many animations from the project I did not use, they are such delicate and well-made, it is pleasure to work with those animations, and 100% deserve a more exquisite system to handle them. The vibration and the adaptive triggers from ps5 controller will be awesome, I am not even put my hands on the execution animation, the parry system will be also amazing, I have seen the animation, is right there! Not to mention the excellent sound effect. I feel like I am a chef and lucky enough to get a ton of top ingredients, but I only have time to do an appetizer. It is top enjoyment to work with those animations, the attacking animation of the katana is the same as the real world, and the model of nodachi is not just a bigger katana, I can imagine this one used on horse riding。


=======================================================================================================================

Climbing:

Critical functions

Tryclimb()
This function call when player want||can climb, the function call the UpdateAverageHit() function to check if the wall frount player climable, if it is, set the average normal. 
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

Shoot 5 line to detect the wall front player wall climbing and trying climbing. Reture false if there not hit, return true if it is hit. Also average the normal of 5 line and set averagehit.normal to it. With the average normal, we can handle complicacy surface.

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

If I have more time to finish this, I will focus on how to make better root motion animation, maybe reset the rotation. Polish the animation and speed. Also, disable the attack during climbing. Set up the attack from the wall, and the landing process, it will be so cool if I can perfectly combine the animation with the climbing. Also, the IK during climbing should be easy to add on, by using Control Rig, but I really don't have time to do it. 









## Questionnaire

I have using unreal engine for about a year, and 2 months about unity.  Most time I am developing is virtual reality application used for education and scientific research in the lab with unreal. I never touch the movement component before,  but spend 2 day to learn it, the climbing is working but still have space to improve. 

The strength for me I will say the ability to abstract the function I try to achieve to a 3d math problem. once it is done, solving the problem is a matter of time. Of course times it will take a lot of time, but most time the mathematician is go further, so I can ask or learn from them. Reading the paper, doing some search, have a conversation will be really helpful. I made an algorithm for the assembly part in the lab, it can assemble lego, engine, and house in VR, it is a thing I completely created by myself, and can be used for different projects, I am proud of it. Also, I believe I have a solid debugging ability, the way I am doing that is to try to understand how everything works, step into/step out/step over. Is like a detective or medical diagnosis. As the only undergraduate RA in the lab, I help a lot with my colleague located the bug in c++. 

The way I sharpen my skills as an engineer is by reading other people's code, understanding why their doing that, try to do the same function in a different way. My PhD colleague teaches me that if he tries to make an innovation, he has to start to read the paper that already exists. Also as a developer related to 3D, math is the best tool to use, a solid math foundation can really save a day. And doing an actual project can combine all the things above, and every time I can learn new stuff when I doing the project.

As a software engineer, I strongly believe interests and passions are most important. No matter how high the salary at google is, the best guy always choice going to NASA. I saw people quit their job at Intel for a small Indie Game studio. I believe it is totally worthwhile, different than any other software engineer, the game developers actually build something else than software, it is something warm and soft, not just a cold machine. I love to spend time there, the video game is a privilege for modern people and I like to actually do something about it with professional people.