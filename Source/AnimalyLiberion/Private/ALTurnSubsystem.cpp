#include "ALTurnSubsystem.h"

#include "ALStatsComponent.h"
#include "ALTurnActorInterface.h"
#include "GameFramework/Actor.h"

void UALTurnSubsystem::RegisterCombatant(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	Combatants.AddUnique(Actor);
}

void UALTurnSubsystem::UnregisterCombatant(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	Combatants.Remove(Actor);
	InitiativeOrder.Remove(Actor);
	if (GetCurrentActor() == Actor)
	{
		EndCurrentTurn();
	}
}

void UALTurnSubsystem::StartCombat()
{
	bCombatActive = true;
	RebuildInitiative();
	CurrentIndex = 0;
	BeginTurnForCurrent();
}

void UALTurnSubsystem::EndCurrentTurn()
{
	if (!bCombatActive)
	{
		return;
	}

	AActor* Current = GetCurrentActor();
	if (Current && Current->GetClass()->ImplementsInterface(UALTurnActorInterface::StaticClass()))
	{
		IALTurnActorInterface::Execute_OnTurnEnded(Current);
	}

	AdvanceToNextActor();
}

AActor* UALTurnSubsystem::GetCurrentActor() const
{
	if (!InitiativeOrder.IsValidIndex(CurrentIndex))
	{
		return nullptr;
	}
	return InitiativeOrder[CurrentIndex].Get();
}

bool UALTurnSubsystem::IsActorsTurn(const AActor* Actor) const
{
	return Actor && Actor == GetCurrentActor();
}

void UALTurnSubsystem::ConsumeTurnByAction(AActor* Actor)
{
	if (!Actor || !IsActorsTurn(Actor))
	{
		return;
	}
	EndCurrentTurn();
}

UALStatsComponent* UALTurnSubsystem::FindStats(const AActor* Actor) const
{
	return Actor ? Actor->FindComponentByClass<UALStatsComponent>() : nullptr;
}

float UALTurnSubsystem::GetSpeed(const AActor* Actor) const
{
	const UALStatsComponent* Stats = FindStats(Actor);
	return Stats ? Stats->Stats.Speed : 0.f;
}

void UALTurnSubsystem::RebuildInitiative()
{
	InitiativeOrder = Combatants;

	InitiativeOrder.Sort([this](const TObjectPtr<AActor>& A, const TObjectPtr<AActor>& B)
	{
		const float SA = GetSpeed(A.Get());
		const float SB = GetSpeed(B.Get());
		if (FMath::IsNearlyEqual(SA, SB))
		{
			return A.Get() < B.Get();
		}
		return SA > SB;
	});
}

void UALTurnSubsystem::BeginTurnForCurrent()
{
	AActor* Current = GetCurrentActor();
	if (!Current)
	{
		return;
	}

	UALStatsComponent* Stats = FindStats(Current);
	if (Stats && Stats->RemainingSkipTurns > 0)
	{
		// "精神体力0の特殊処理" 等で発生するスキップを、このターン開始時に消費して即終了。
		Stats->RemainingSkipTurns--;
		EndCurrentTurn();
		return;
	}

	if (Current->GetClass()->ImplementsInterface(UALTurnActorInterface::StaticClass()))
	{
		const bool bAble = IALTurnActorInterface::Execute_IsAbleToAct(Current);
		IALTurnActorInterface::Execute_OnTurnBegan(Current);
		if (!bAble || (Stats && !Stats->CanActThisTurn()))
		{
			EndCurrentTurn();
		}
	}
}

void UALTurnSubsystem::AdvanceToNextActor()
{
	if (InitiativeOrder.Num() == 0)
	{
		bCombatActive = false;
		CurrentIndex = INDEX_NONE;
		return;
	}

	CurrentIndex++;
	if (CurrentIndex >= InitiativeOrder.Num())
	{
		RebuildInitiative();
		CurrentIndex = 0;
	}
	BeginTurnForCurrent();
}

