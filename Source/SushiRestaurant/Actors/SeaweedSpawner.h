// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/IngredientSpawner.h"
#include "SeaweedSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API ASeaweedSpawner : public AIngredientSpawner
{
	GENERATED_BODY()

public:
	ASeaweedSpawner()
	{
		SpawnIngredientType = EIngredientType::Seaweed;
	}
};
