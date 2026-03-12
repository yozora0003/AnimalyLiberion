#include "ALCombatCharacter.h"

#include "ALAttackSkill.h"
#include "ALSkillSlotsComponent.h"
#include "ALStatsComponent.h"
#include "ALTurnSubsystem.h"

AALCombatCharacter::AALCombatCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	StatsComponent = CreateDefaultSubobject<UALStatsComponent>(TEXT("Stats"));
	SkillSlotsComponent = CreateDefaultSubobject<UALSkillSlotsComponent>(TEXT("SkillSlots"));
}

void AALCombatCharacter::BeginPlay()
{
	Super::BeginPlay();
}

UALTurnSubsystem* AALCombatCharacter::GetTurnSubsystem() const
{
	if (!GetWorld())
	{
		return nullptr;
	}
	return GetWorld()->GetSubsystem<UALTurnSubsystem>();
}

bool AALCombatCharacter::UseSkillSlot(int32 SlotIndex, AActor* Target)
{
	UALTurnSubsystem* Turn = GetTurnSubsystem();
	if (!Turn || !Turn->IsActorsTurn(this) || !SkillSlotsComponent)
	{
		return false;
	}

	UALAttackSkill* Skill = SkillSlotsComponent->GetSkillInSlot(SlotIndex);
	if (!Skill || !Skill->CanUseOn(this, Target))
	{
		return false;
	}

	Skill->UseSkill(this, Target);
	Turn->ConsumeTurnByAction(this);
	return true;
}

void AALCombatCharacter::ConsumeTurn()
{
	if (UALTurnSubsystem* Turn = GetTurnSubsystem())
	{
		Turn->ConsumeTurnByAction(this);
	}
}

UALStatsComponent* AALCombatCharacter::GetStatsComponent_Implementation() const
{
	return StatsComponent;
}

bool AALCombatCharacter::IsAbleToAct_Implementation() const
{
	return StatsComponent ? StatsComponent->CanActThisTurn() : true;
}

void AALCombatCharacter::OnTurnBegan_Implementation()
{
	// Intended for Blueprint/UI hooks.
}

void AALCombatCharacter::OnTurnEnded_Implementation()
{
	// Intended for Blueprint/UI hooks.
}

