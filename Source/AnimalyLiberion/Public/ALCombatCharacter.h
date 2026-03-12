#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ALTurnActorInterface.h"
#include "ALCombatCharacter.generated.h"

class UALStatsComponent;
class UALSkillSlotsComponent;
class UALAttackSkill;
class UALTurnSubsystem;

UCLASS()
class AALCombatCharacter : public ACharacter, public IALTurnActorInterface
{
	GENERATED_BODY()

public:
	AALCombatCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UALStatsComponent> StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UALSkillSlotsComponent> SkillSlotsComponent;

	UFUNCTION(BlueprintCallable, Category = "Turn")
	UALTurnSubsystem* GetTurnSubsystem() const;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool UseSkillSlot(int32 SlotIndex, AActor* Target);

	// Turn-based move: you can implement pathing in BP and call ConsumeTurn.
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void ConsumeTurn();

	// --- IALTurnActorInterface ---
	virtual UALStatsComponent* GetStatsComponent_Implementation() const override;
	virtual bool IsAbleToAct_Implementation() const override;
	virtual void OnTurnBegan_Implementation() override;
	virtual void OnTurnEnded_Implementation() override;

protected:
	virtual void BeginPlay() override;
};

