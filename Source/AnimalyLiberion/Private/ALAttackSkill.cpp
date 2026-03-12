#include "ALAttackSkill.h"

#include "ALStatsComponent.h"
#include "GameFramework/Actor.h"

UALStatsComponent* UALAttackSkill::FindStats(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}
	return Actor->FindComponentByClass<UALStatsComponent>();
}

bool UALAttackSkill::CanUseOn(const AActor* User, const AActor* Target) const
{
	if (!User || !Target || User == Target)
	{
		return false;
	}

	const float Dist = FVector::Distance(User->GetActorLocation(), Target->GetActorLocation());
	if (Dist > Range)
	{
		return false;
	}

	const UALStatsComponent* UserStats = FindStats(User);
	const UALStatsComponent* TargetStats = FindStats(Target);
	return UserStats && TargetStats;
}

FALDamageResult UALAttackSkill::UseSkill(AActor* User, AActor* Target) const
{
	FALDamageResult Result;
	Result.DamageType = DamageType;

	UALStatsComponent* UserStats = FindStats(User);
	UALStatsComponent* TargetStats = FindStats(Target);
	if (!User || !Target || !UserStats || !TargetStats)
	{
		return Result;
	}

	const float AttackPower =
		(DamageType == EALDamageType::Physical) ? UserStats->Stats.PhysicalAttack : UserStats->Stats.MentalAttack;
	const float DefensePower =
		(DamageType == EALDamageType::Physical) ? TargetStats->Stats.PhysicalDefense : TargetStats->Stats.MentalDefense;

	const float Raw = UALStatsComponent::CalculateRawDamage(DamageType, AttackPower, DefensePower, PowerMultiplier);
	Result = TargetStats->ApplyDamage(DamageType, Raw);
	return Result;
}

