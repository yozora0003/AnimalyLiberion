#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALCombatTypes.h"
#include "ALTurnActorInterface.generated.h"

class UALStatsComponent;

UINTERFACE(Blueprintable)
class UALTurnActorInterface : public UInterface
{
	GENERATED_BODY()
};

class IALTurnActorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Turn")
	UALStatsComponent* GetStatsComponent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Turn")
	bool IsAbleToAct() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Turn")
	void OnTurnBegan();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Turn")
	void OnTurnEnded();
};

