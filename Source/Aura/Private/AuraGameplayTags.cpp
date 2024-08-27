// Copyright QiuXuan


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(Manager);
	// Notify manager that we are done adding native tags.
	Manager.DoneAddingNativeTags();
}

void FAuraGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// Attribute
	AddTag(Attributes_Primary_Strength, "Attributes.Primary.Strength", "Increases physical damage");
	AddTag(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence", "Increases magical damage");
	AddTag(Attributes_Primary_Resilience, "Attributes.Primary.Resilience", "Increases armor and armor penetration");
	AddTag(Attributes_Primary_Vigor, "Attributes.Primary.Vigor", "Increases health");

	AddTag(Attributes_Secondary_Armor, "Attributes.Secondary.Armor", "Reduces damage taken, improves Block Chance");
	AddTag(Attributes_Secondary_ArmorPenetration, "Attributes.Secondary.ArmorPenetration", "Ignores Percentage of enemy Armor, increases Critical Hit Chance");
	AddTag(Attributes_Secondary_BlockChance, "Attributes.Secondary.BlockChance", "Chance to cut incoming damage in half");
	AddTag(Attributes_Secondary_CriticalHitChance, "Attributes.Secondary.CriticalHitChance", "Chance to double damage plus critical hit bonus");
	AddTag(Attributes_Secondary_CriticalHitDamage, "Attributes.Secondary.CriticalHitDamage", "Bonus damage added when a critical hit is scored");
	AddTag(Attributes_Secondary_CriticalHitResistance, "Attributes.Secondary.CriticalHitResistance", "Reduces critical hit chance of attacking enemies");
	AddTag(Attributes_Secondary_HealthRegeneration, "Attributes.Secondary.HealthRegeneration", "Amount of Health regenerated every 1 second");
	AddTag(Attributes_Secondary_ManaRegeneration, "Attributes.Secondary.ManaRegeneration", "Amount of Mana regenerated every 1 second");
	AddTag(Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth", "Maximum amount of Health obtainable");
	AddTag(Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana", "Maximum amount of Mana obtainable");

	// Input
	AddTag(InputTag_LMB, "InputTag.LMB", "Input Tag for Left Mouse Button");
	AddTag(InputTag_RMB, "InputTag.RMB", "Input Tag for Right Mouse Button");
	AddTag(InputTag_1, "InputTag.1", "Input Tag for Num 1 Key");
	AddTag(InputTag_2, "InputTag.2", "Input Tag for Num 2 Key");
	AddTag(InputTag_3, "InputTag.3", "Input Tag for Num 3 Key");
	AddTag(InputTag_4, "InputTag.4", "Input Tag for Num 4 Key");

	// Damage
	AddTag(Damage, "Damage", "Assign Tag Set By Caller Damage Magnitude");
	AddTag(Damage_Fire, "Damage.Fire", "Fire Damage Type");
	AddTag(Damage_Lightning, "Damage.Lightning", "Lightning Damage Type");
	AddTag(Damage_Arcane, "Damage.Arcane", "Arcane Damage Type");
	AddTag(Damage_Physical, "Damage.Physical", "Physical Damage Type");

	// Damage Resistance
	AddTag(Attributes_Resistance_Fire, "Attributes.Resistance.Fire", "Resistance to Fire Damage");
	AddTag(Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning", "Resistance to Lightning Damage");
	AddTag(Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane", "Resistance to Arcane Damage");
	AddTag(Attributes_Resistance_Physical, "Attributes.Resistance.Physical", "Resistance to Physical Damage");

	// Map Damage Types to Resistance
	DamageTypesToResistance.Emplace(Damage_Fire, Attributes_Resistance_Fire);
	DamageTypesToResistance.Emplace(Damage_Lightning, Attributes_Resistance_Lightning);
	DamageTypesToResistance.Emplace(Damage_Arcane, Attributes_Resistance_Arcane);
	DamageTypesToResistance.Emplace(Damage_Physical, Attributes_Resistance_Physical);
	
	// Effects
	AddTag(Effects_HitReact, "Effects.HitReact", "Enemies React to Hit");

	// Attack
	AddTag(Abilities_Attack, "Abilities.Attack", "Attack Ability Tag");

	// Attack
	AddTag(Montage_Attack_Weapon, "Montage.Attack.Weapon", "Weapon");
	AddTag(Montage_Attack_LeftHand, "Montage.Attack.LeftHand", "LeftHand");
	AddTag(Montage_Attack_RightHand, "Montage.Attack.RightHand", "RightHand");
	
}

void FAuraGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

