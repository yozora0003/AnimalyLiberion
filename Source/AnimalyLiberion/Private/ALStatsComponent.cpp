#include "ALStatsComponent.h"

UALStatsComponent::UALStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UALStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	Stats.PhysicalHP = FMath::Clamp(Stats.PhysicalHP, 0.f, Stats.PhysicalHPMax);
	Stats.MentalHP = FMath::Clamp(Stats.MentalHP, 0.f, Stats.MentalHPMax);
}

void UALStatsComponent::ResetToMax()
{
	Stats.PhysicalHP = Stats.PhysicalHPMax;
	Stats.MentalHP = Stats.MentalHPMax;
	bMentalBroken = false;
	RemainingSkipTurns = 0;
}

bool UALStatsComponent::IsPhysicallyAlive() const
{
	return Stats.PhysicalHP > 0.f;
}

bool UALStatsComponent::IsMentallyAlive() const
{
	return Stats.MentalHP > 0.f;
}

bool UALStatsComponent::CanActThisTurn() const
{
	return RemainingSkipTurns <= 0;
}

void UALStatsComponent::ConsumeSkipTurnIfNeeded()
{
	if (RemainingSkipTurns > 0)
	{
		RemainingSkipTurns--;
	}
}

float UALStatsComponent::CalculateRawDamage(EALDamageType DamageType, float AttackPower, float DefensePower, float PowerMultiplier)
{
	const float Base = FMath::Max(1.f, AttackPower - DefensePower);
	const float Mult = FMath::Max(0.f, PowerMultiplier);
	return Base * Mult;
}

FALDamageResult UALStatsComponent::ApplyDamage(EALDamageType DamageType, float RawDamage)
{
	FALDamageResult Result;
	Result.DamageType = DamageType;
	Result.RawDamage = RawDamage;

	const float Applied = FMath::Max(0.f, RawDamage);
	Result.AppliedDamage = Applied;

	if (DamageType == EALDamageType::Physical)
	{
		Stats.PhysicalHP = FMath::Clamp(Stats.PhysicalHP - Applied, 0.f, Stats.PhysicalHPMax);
	}
	else
	{
		Stats.MentalHP = FMath::Clamp(Stats.MentalHP - Applied, 0.f, Stats.MentalHPMax);
		if (Stats.MentalHP <= 0.f)
		{
			HandleMentalZero();
		}
	}

	return Result;
}

void UALStatsComponent::HandleMentalZero()
{
	if (bMentalBroken)
	{
		return;
	}

	bMentalBroken = true;
	RemainingSkipTurns = FMath::Max(0, MentalBreakSkipTurns);
	OnMentalBreak.Broadcast(GetOwner());
}

