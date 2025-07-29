// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/GameTimeManager.h"

// Sets default values
AGameTimeManager::AGameTimeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameTimeManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameTimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

