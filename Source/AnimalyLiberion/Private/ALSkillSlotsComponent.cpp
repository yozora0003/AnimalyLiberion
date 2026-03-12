#include "ALSkillSlotsComponent.h"

#include "ALAttackSkill.h"

UALSkillSlotsComponent::UALSkillSlotsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	EnsureSize();
}

void UALSkillSlotsComponent::BeginPlay()
{
	Super::BeginPlay();
	EnsureSize();
}

void UALSkillSlotsComponent::EnsureSize()
{
	if (SkillSlots.Num() != NumSlots)
	{
		SkillSlots.SetNum(NumSlots);
	}
}

UALAttackSkill* UALSkillSlotsComponent::GetSkillInSlot(int32 SlotIndex) const
{
	if (!SkillSlots.IsValidIndex(SlotIndex))
	{
		return nullptr;
	}
	return SkillSlots[SlotIndex];
}

bool UALSkillSlotsComponent::SetSkillInSlot(int32 SlotIndex, UALAttackSkill* Skill)
{
	EnsureSize();
	if (!SkillSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}
	SkillSlots[SlotIndex] = Skill;
	return true;
}

