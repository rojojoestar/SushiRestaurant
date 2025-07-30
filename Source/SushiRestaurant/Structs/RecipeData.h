#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h" // for EIngredientType
#include "RecipeData.generated.h"

class URecipeAsset;

/** Common cookware types used by steps. */
UENUM(BlueprintType)
enum class ECookwareType : uint8
{
	CuttingBoard UMETA(DisplayName="Cutting Board"),
	Stove        UMETA(DisplayName="Stove"),
	MixingTable  UMETA(DisplayName="Mixing Table"),
	None         UMETA(DisplayName="None")
};

/** Top-level recipe data used by UI and validation. */
USTRUCT(BlueprintType)
struct FRecipeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recipe")
	FText RecipeName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recipe")
	TArray<EIngredientType> RequiredIngredients;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Recipe")
	UTexture2D* RecipeIcon = nullptr;
};

/** One step of a recipe pipeline (optional extension). */
USTRUCT(BlueprintType)
struct FRecipeStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
	TSubclassOf<URecipeAsset> InputIngredient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
	TSubclassOf<URecipeAsset> OutputIngredient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
	float ProcessingTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Recipe")
	ECookwareType RequiredStation = ECookwareType::None;
};
