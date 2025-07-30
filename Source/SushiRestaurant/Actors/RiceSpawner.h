// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/IngredientSpawner.h"
#include "RiceSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SUSHIRESTAURANT_API ARiceSpawner : public AIngredientSpawner
{
	GENERATED_BODY()
public:
	ARiceSpawner()
	{
		SpawnIngredientType = EIngredientType::Rice;
	}
	
};
