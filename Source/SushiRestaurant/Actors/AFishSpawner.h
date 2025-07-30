// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/IngredientSpawner.h"
#include "AFishSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API AAFishSpawner : public AIngredientSpawner
{
	GENERATED_BODY()

public:
	AAFishSpawner()
	{
		SpawnIngredientType = EIngredientType::Fish;
	}
	
};
