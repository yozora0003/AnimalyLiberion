#pragma once

#include "CoreMinimal.h"
#include "ALCombatCharacter.h"
#include "ALAICombatCharacter.generated.h"

class UALMinimaxAIComponent;

UCLASS()
class AALAICombatCharacter : public AALCombatCharacter
{
	GENERATED_BODY()

public:
	AALAICombatCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UALMinimaxAIComponent> MinimaxAI;

protected:
	virtual void OnTurnBegan_Implementation() override;
};

