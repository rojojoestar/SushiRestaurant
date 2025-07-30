#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/RecipeData.h"
#include "RecipeAsset.generated.h"

/**
 * Data asset describing a recipe (name, required ingredients, and step sequence).
 */
UCLASS(BlueprintType)
class URecipeAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// High-level recipe data (name, icon, required ingredients)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRecipeData Recipe;

	// Ordered processing steps (optional extension)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRecipeStep> Steps;
};
