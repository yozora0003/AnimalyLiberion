#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALCombatTypes.h"
#include "ALMinimaxAIComponent.generated.h"

class AALCombatCharacter;
class UALAttackSkill;

USTRUCT()
struct FALSimStats
{
	GENERATED_BODY()

	float PhysicalHP = 0.f;
	float MentalHP = 0.f;
	float PhysicalAttack = 0.f;
	float PhysicalDefense = 0.f;
	float MentalAttack = 0.f;
	float MentalDefense = 0.f;
	bool bMentalBroken = false;
};

UENUM()
enum class EALSimAction : uint8
{
	Wait,
	MoveCloser,
	UseSlot0,
	UseSlot1,
	UseSlot2,
	UseSlot3,
};

USTRUCT()
struct FALSimState
{
	GENERATED_BODY()

	FALSimStats Enemy;
	FALSimStats Player;
	float Distance = 0.f;
	bool bEnemyToAct = true;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UALMinimaxAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UALMinimaxAIComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(ClampMin="1"))
	int32 SearchDepth = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(ClampMin="0"))
	float MoveStep = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<AActor> TargetActor;

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool TakeTurn();

private:
	AALCombatCharacter* GetOwnerCombatCharacter() const;
	AALCombatCharacter* GetTargetCombatCharacter() const;

	FALSimState BuildSimState() const;

	TArray<EALSimAction> GetLegalActions(const FALSimState& State) const;
	FALSimState ApplyAction(const FALSimState& State, EALSimAction Action) const;

	float Evaluate(const FALSimState& State) const;
	float Minimax(const FALSimState& State, int32 Depth, float Alpha, float Beta) const;
	EALSimAction ChooseBestAction(const FALSimState& State) const;

	float GetSkillRange(EALSimAction Action) const;
	const UALAttackSkill* GetSkillForAction(EALSimAction Action) const;
	static float SimDamage(EALDamageType DamageType, float Attack, float Defense, float Mult);

	bool ExecuteActionRuntime(EALSimAction Action);
};

