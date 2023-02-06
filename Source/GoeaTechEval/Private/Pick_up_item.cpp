// Fill out your copyright notice in the Description page of Project Settings.

#include "Pick_up_item.h"
#include "Components/SphereComponent.h"
//#include "NiagaraComponent.h"
//#include "NiagaraFunctionLibrary.h"
#include "UObject/Interface.h"
//#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APick_up_item::APick_up_item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APick_up_item::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &APick_up_item::OnSphereEndOverlap);
}

// Called when the game starts or when spawned
void APick_up_item::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void APick_up_item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APick_up_item::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void APick_up_item::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void APick_up_item::SpawnPickupSystem()
{
}

void APick_up_item::SpawnPickupSound()
{
}

