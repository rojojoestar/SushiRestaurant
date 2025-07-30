// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/GameTimeManager.h"

AGameTimeManager::AGameTimeManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameTimeManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameTimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

