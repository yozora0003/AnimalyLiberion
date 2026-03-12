#include "ALAICombatCharacter.h"

#include "ALMinimaxAIComponent.h"

AALAICombatCharacter::AALAICombatCharacter()
{
	MinimaxAI = CreateDefaultSubobject<UALMinimaxAIComponent>(TEXT("MinimaxAI"));
}

void AALAICombatCharacter::OnTurnBegan_Implementation()
{
	Super::OnTurnBegan_Implementation();

	if (MinimaxAI)
	{
		MinimaxAI->TakeTurn();
	}
}

