// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameTimeManager.generated.h"

/**
 * Placeholder actor for future central time management (if needed).
 */
UCLASS()
class SUSHIRESTAURANT_API AGameTimeManager : public AActor
{
	GENERATED_BODY()

public:
	AGameTimeManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};

