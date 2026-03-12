#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ALCombatTypes.h"
#include "ALAttackSkill.generated.h"

class UALStatsComponent;

UCLASS(BlueprintType)
class UALAttackSkill : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FName SkillId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EALDamageType DamageType = EALDamageType::Physical;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta=(ClampMin="0"))
	float PowerMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta=(ClampMin="0"))
	float Range = 200.f;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool CanUseOn(const AActor* User, const AActor* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	FALDamageResult UseSkill(AActor* User, AActor* Target) const;

private:
	static UALStatsComponent* FindStats(const AActor* Actor);
};

