// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "RecipeData.generated.h"

USTRUCT(BlueprintType)
struct FRecipeData
{
	GENERATED_BODY()

	// Name of the recipe
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText RecipeName;

	// List of required ingredients
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EIngredientType> RequiredIngredients;

	// Optional: Icon or image for the recipe
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* RecipeIcon;
};
