#include "ALMinimaxAIComponent.h"

#include "ALAttackSkill.h"
#include "ALCombatCharacter.h"
#include "ALSkillSlotsComponent.h"
#include "ALStatsComponent.h"
#include "ALTurnSubsystem.h"
#include "AIController.h"

UALMinimaxAIComponent::UALMinimaxAIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AALCombatCharacter* UALMinimaxAIComponent::GetOwnerCombatCharacter() const
{
	return Cast<AALCombatCharacter>(GetOwner());
}

AALCombatCharacter* UALMinimaxAIComponent::GetTargetCombatCharacter() const
{
	return Cast<AALCombatCharacter>(TargetActor.Get());
}

bool UALMinimaxAIComponent::TakeTurn()
{
	AALCombatCharacter* OwnerChar = GetOwnerCombatCharacter();
	if (!OwnerChar)
	{
		return false;
	}

	UALTurnSubsystem* Turn = OwnerChar->GetTurnSubsystem();
	if (!Turn || !Turn->IsActorsTurn(OwnerChar))
	{
		return false;
	}

	FALSimState State = BuildSimState();
	EALSimAction Best = ChooseBestAction(State);
	return ExecuteActionRuntime(Best);
}

FALSimState UALMinimaxAIComponent::BuildSimState() const
{
	FALSimState S;
	AALCombatCharacter* OwnerChar = GetOwnerCombatCharacter();
	AALCombatCharacter* TargetChar = GetTargetCombatCharacter();
	if (!OwnerChar || !TargetChar || !OwnerChar->StatsComponent || !TargetChar->StatsComponent)
	{
		return S;
	}

	const FALStats& E = OwnerChar->StatsComponent->Stats;
	S.Enemy.PhysicalHP = E.PhysicalHP;
	S.Enemy.MentalHP = E.MentalHP;
	S.Enemy.PhysicalAttack = E.PhysicalAttack;
	S.Enemy.PhysicalDefense = E.PhysicalDefense;
	S.Enemy.MentalAttack = E.MentalAttack;
	S.Enemy.MentalDefense = E.MentalDefense;
	S.Enemy.bMentalBroken = OwnerChar->StatsComponent->bMentalBroken;

	const FALStats& P = TargetChar->StatsComponent->Stats;
	S.Player.PhysicalHP = P.PhysicalHP;
	S.Player.MentalHP = P.MentalHP;
	S.Player.PhysicalAttack = P.PhysicalAttack;
	S.Player.PhysicalDefense = P.PhysicalDefense;
	S.Player.MentalAttack = P.MentalAttack;
	S.Player.MentalDefense = P.MentalDefense;
	S.Player.bMentalBroken = TargetChar->StatsComponent->bMentalBroken;

	S.Distance = FVector::Distance(OwnerChar->GetActorLocation(), TargetChar->GetActorLocation());
	S.bEnemyToAct = true;
	return S;
}

float UALMinimaxAIComponent::SimDamage(EALDamageType DamageType, float Attack, float Defense, float Mult)
{
	const float Base = FMath::Max(1.f, Attack - Defense);
	return Base * FMath::Max(0.f, Mult);
}

const UALAttackSkill* UALMinimaxAIComponent::GetSkillForAction(EALSimAction Action) const
{
	const AALCombatCharacter* OwnerChar = GetOwnerCombatCharacter();
	if (!OwnerChar || !OwnerChar->SkillSlotsComponent)
	{
		return nullptr;
	}

	const int32 Slot =
		(Action == EALSimAction::UseSlot0) ? 0 :
		(Action == EALSimAction::UseSlot1) ? 1 :
		(Action == EALSimAction::UseSlot2) ? 2 :
		(Action == EALSimAction::UseSlot3) ? 3 : INDEX_NONE;

	if (Slot == INDEX_NONE)
	{
		return nullptr;
	}
	return OwnerChar->SkillSlotsComponent->GetSkillInSlot(Slot);
}

float UALMinimaxAIComponent::GetSkillRange(EALSimAction Action) const
{
	const UALAttackSkill* Skill = GetSkillForAction(Action);
	return Skill ? Skill->Range : 0.f;
}

TArray<EALSimAction> UALMinimaxAIComponent::GetLegalActions(const FALSimState& State) const
{
	TArray<EALSimAction> Actions;
	Actions.Add(EALSimAction::Wait);
	Actions.Add(EALSimAction::MoveCloser);

	const float Dist = State.Distance;
	for (EALSimAction A : {EALSimAction::UseSlot0, EALSimAction::UseSlot1, EALSimAction::UseSlot2, EALSimAction::UseSlot3})
	{
		const UALAttackSkill* Skill = GetSkillForAction(A);
		if (!Skill)
		{
			continue;
		}
		if (Dist <= Skill->Range)
		{
			Actions.Add(A);
		}
	}

	return Actions;
}

FALSimState UALMinimaxAIComponent::ApplyAction(const FALSimState& State, EALSimAction Action) const
{
	FALSimState Next = State;

	if (Action == EALSimAction::Wait)
	{
		Next.bEnemyToAct = !State.bEnemyToAct;
		return Next;
	}

	if (Action == EALSimAction::MoveCloser)
	{
		Next.Distance = FMath::Max(0.f, State.Distance - MoveStep);
		Next.bEnemyToAct = !State.bEnemyToAct;
		return Next;
	}

	// Use skill action (enemy uses its own slots; player reply is approximated separately)
	const UALAttackSkill* Skill = GetSkillForAction(Action);
	if (!Skill)
	{
		Next.bEnemyToAct = !State.bEnemyToAct;
		return Next;
	}

	const EALDamageType DT = Skill->DamageType;
	const float Mult = Skill->PowerMultiplier;

	if (State.bEnemyToAct)
	{
		const float Atk = (DT == EALDamageType::Physical) ? State.Enemy.PhysicalAttack : State.Enemy.MentalAttack;
		const float Def = (DT == EALDamageType::Physical) ? State.Player.PhysicalDefense : State.Player.MentalDefense;
		const float Dmg = SimDamage(DT, Atk, Def, Mult);

		if (DT == EALDamageType::Physical)
		{
			Next.Player.PhysicalHP = FMath::Max(0.f, Next.Player.PhysicalHP - Dmg);
		}
		else
		{
			Next.Player.MentalHP = FMath::Max(0.f, Next.Player.MentalHP - Dmg);
			if (Next.Player.MentalHP <= 0.f)
			{
				Next.Player.bMentalBroken = true;
			}
		}
	}
	else
	{
		// For minimax, we approximate player has "best response" handled in Minimax.
	}

	Next.bEnemyToAct = !State.bEnemyToAct;
	return Next;
}

float UALMinimaxAIComponent::Evaluate(const FALSimState& State) const
{
	// Positive is good for enemy.
	const float EnemyHP = State.Enemy.PhysicalHP + State.Enemy.MentalHP;
	const float PlayerHP = State.Player.PhysicalHP + State.Player.MentalHP;

	const float PlayerMindPenalty = State.Player.bMentalBroken ? 50.f : 0.f;
	const float EnemyMindPenalty = State.Enemy.bMentalBroken ? 50.f : 0.f;

	return (EnemyHP - EnemyMindPenalty) - (PlayerHP - PlayerMindPenalty);
}

float UALMinimaxAIComponent::Minimax(const FALSimState& InState, int32 Depth, float Alpha, float Beta) const
{
	if (Depth <= 0)
	{
		return Evaluate(InState);
	}

	// Terminal-ish: if either side's both pools are 0.
	const bool bEnemyDown = (InState.Enemy.PhysicalHP <= 0.f) && (InState.Enemy.MentalHP <= 0.f);
	const bool bPlayerDown = (InState.Player.PhysicalHP <= 0.f) && (InState.Player.MentalHP <= 0.f);
	if (bEnemyDown || bPlayerDown)
	{
		return Evaluate(InState);
	}

	if (InState.bEnemyToAct)
	{
		float Best = -FLT_MAX;
		const TArray<EALSimAction> Actions = GetLegalActions(InState);
		for (EALSimAction A : Actions)
		{
			FALSimState Next = ApplyAction(InState, A);
			const float Score = Minimax(Next, Depth - 1, Alpha, Beta);
			Best = FMath::Max(Best, Score);
			Alpha = FMath::Max(Alpha, Best);
			if (Beta <= Alpha)
			{
				break;
			}
		}
		return Best;
	}

	// Player turn: approximate "best response" via greedy best-damage skill (or move closer).
	{
		FALSimState Next = InState;

		// If in range of any enemy skill, assume player can also hit with a "best" action:
		// We model player response as max damage among physical/mental.
		float BestDmg = 0.f;

		// Physical response
		BestDmg = FMath::Max(BestDmg, SimDamage(EALDamageType::Physical, Next.Player.PhysicalAttack, Next.Enemy.PhysicalDefense, 1.f));
		// Mental response
		BestDmg = FMath::Max(BestDmg, SimDamage(EALDamageType::Mental, Next.Player.MentalAttack, Next.Enemy.MentalDefense, 1.f));

		if (Next.Distance > 200.f)
		{
			Next.Distance = FMath::Max(0.f, Next.Distance - MoveStep);
		}
		else
		{
			// Apply best damage to enemy; pick whether it hits physical or mental pool by which damage chosen.
			const float PhysDmg = SimDamage(EALDamageType::Physical, Next.Player.PhysicalAttack, Next.Enemy.PhysicalDefense, 1.f);
			const float MentDmg = SimDamage(EALDamageType::Mental, Next.Player.MentalAttack, Next.Enemy.MentalDefense, 1.f);
			if (MentDmg >= PhysDmg)
			{
				Next.Enemy.MentalHP = FMath::Max(0.f, Next.Enemy.MentalHP - MentDmg);
				if (Next.Enemy.MentalHP <= 0.f)
				{
					Next.Enemy.bMentalBroken = true;
				}
			}
			else
			{
				Next.Enemy.PhysicalHP = FMath::Max(0.f, Next.Enemy.PhysicalHP - PhysDmg);
			}
		}

		Next.bEnemyToAct = true;
		return Minimax(Next, Depth - 1, Alpha, Beta);
	}
}

EALSimAction UALMinimaxAIComponent::ChooseBestAction(const FALSimState& State) const
{
	EALSimAction BestAction = EALSimAction::Wait;
	float BestScore = -FLT_MAX;

	const TArray<EALSimAction> Actions = GetLegalActions(State);
	for (EALSimAction A : Actions)
	{
		FALSimState Next = ApplyAction(State, A);
		const float Score = Minimax(Next, SearchDepth - 1, -FLT_MAX, FLT_MAX);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestAction = A;
		}
	}
	return BestAction;
}

bool UALMinimaxAIComponent::ExecuteActionRuntime(EALSimAction Action)
{
	AALCombatCharacter* OwnerChar = GetOwnerCombatCharacter();
	AALCombatCharacter* TargetChar = GetTargetCombatCharacter();
	if (!OwnerChar)
	{
		return false;
	}

	if (Action == EALSimAction::Wait)
	{
		OwnerChar->ConsumeTurn();
		return true;
	}

	if (Action == EALSimAction::MoveCloser)
	{
		if (TargetChar)
		{
			if (AAIController* AIC = Cast<AAIController>(OwnerChar->GetController()))
			{
				AIC->MoveToActor(TargetChar, 100.f);
			}
		}
		OwnerChar->ConsumeTurn();
		return true;
	}

	const int32 Slot =
		(Action == EALSimAction::UseSlot0) ? 0 :
		(Action == EALSimAction::UseSlot1) ? 1 :
		(Action == EALSimAction::UseSlot2) ? 2 :
		(Action == EALSimAction::UseSlot3) ? 3 : INDEX_NONE;

	if (Slot == INDEX_NONE)
	{
		OwnerChar->ConsumeTurn();
		return true;
	}

	return OwnerChar->UseSkillSlot(Slot, TargetChar);
}

