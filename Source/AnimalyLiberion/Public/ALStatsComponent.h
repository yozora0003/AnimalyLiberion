#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALCombatTypes.h"
#include "ALStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALOnMentalBreak, AActor*, Actor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UALStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UALStatsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FALStats Stats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bMentalBroken = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", meta=(ClampMin="0"))
	int32 MentalBreakSkipTurns = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta=(ClampMin="0"))
	int32 RemainingSkipTurns = 0;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FALOnMentalBreak OnMentalBreak;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ResetToMax();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsPhysicallyAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsMentallyAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool CanActThisTurn() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ConsumeSkipTurnIfNeeded();

	UFUNCTION(BlueprintCallable, Category = "Damage")
	FALDamageResult ApplyDamage(EALDamageType DamageType, float RawDamage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static float CalculateRawDamage(EALDamageType DamageType, float AttackPower, float DefensePower, float PowerMultiplier = 1.f);

protected:
	virtual void BeginPlay() override;

private:
	void HandleMentalZero();
};

