// Fill out your copyright notice in the Description page of Project Settings.

// RecipeAsset.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/RecipeData.h"
#include "RecipeAsset.generated.h"

UCLASS(BlueprintType)
class URecipeAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRecipeData Recipe;
};
