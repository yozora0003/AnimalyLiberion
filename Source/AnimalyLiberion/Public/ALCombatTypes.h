#pragma once

#include "CoreMinimal.h"
#include "ALCombatTypes.generated.h"

UENUM(BlueprintType)
enum class EALDamageType : uint8
{
	Physical UMETA(DisplayName = "Physical"),
	Mental UMETA(DisplayName = "Mental"),
};

UENUM(BlueprintType)
enum class EALActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Move UMETA(DisplayName = "Move"),
	UseSkill UMETA(DisplayName = "UseSkill"),
	Wait UMETA(DisplayName = "Wait"),
};

USTRUCT(BlueprintType)
struct FALStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float PhysicalHPMax = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float PhysicalHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MentalHPMax = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MentalHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float PhysicalAttack = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float PhysicalDefense = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MentalAttack = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MentalDefense = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Speed = 10.f;
};

USTRUCT(BlueprintType)
struct FALDamageResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	EALDamageType DamageType = EALDamageType::Physical;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float RawDamage = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	float AppliedDamage = 0.f;
};

