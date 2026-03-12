#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ALTurnSubsystem.generated.h"

class UALStatsComponent;

UCLASS()
class UALTurnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void RegisterCombatant(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void UnregisterCombatant(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void StartCombat();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void EndCurrentTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	AActor* GetCurrentActor() const;

	UFUNCTION(BlueprintCallable, Category = "Turn")
	bool IsActorsTurn(const AActor* Actor) const;

	// Move/Skill usage should call this (or directly EndCurrentTurn).
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void ConsumeTurnByAction(AActor* Actor);

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> Combatants;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> InitiativeOrder;

	int32 CurrentIndex = INDEX_NONE;
	bool bCombatActive = false;

	void RebuildInitiative();
	float GetSpeed(const AActor* Actor) const;
	UALStatsComponent* FindStats(const AActor* Actor) const;

	void BeginTurnForCurrent();
	void AdvanceToNextActor();
};

