#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSkillSlotsComponent.generated.h"

class UALAttackSkill;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UALSkillSlotsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UALSkillSlotsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills", meta=(DisplayName="Skill Slots (4)"))
	TArray<TObjectPtr<UALAttackSkill>> SkillSlots;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	UALAttackSkill* GetSkillInSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	bool SetSkillInSlot(int32 SlotIndex, UALAttackSkill* Skill);

protected:
	virtual void BeginPlay() override;

private:
	static constexpr int32 NumSlots = 4;
	void EnsureSize();
};

