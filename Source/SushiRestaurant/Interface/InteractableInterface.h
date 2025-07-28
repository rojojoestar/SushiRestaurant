#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(APawn* Interactor) = 0;
};
