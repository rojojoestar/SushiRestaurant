// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "RecipeData.generated.h"

class URecipeAsset;

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

UENUM(BlueprintType)
enum class ECookwareType : uint8
{
	CuttingBoard	UMETA(DisplayName = "Cutting Board"),
	Stove			UMETA(DisplayName = "Stove"),
	MixingTable		UMETA(DisplayName = "Mixing Table"),
	None			UMETA(DisplayName = "None")
};


USTRUCT(BlueprintType)
struct FRecipeStep
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	TSubclassOf<URecipeAsset> InputIngredient;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	TSubclassOf<URecipeAsset> OutputIngredient;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	float ProcessingTime = 5.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	ECookwareType RequiredStation = ECookwareType::None;
};
