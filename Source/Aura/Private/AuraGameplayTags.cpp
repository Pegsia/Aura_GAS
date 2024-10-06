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
	AddTag(Attributes_Vital_Health, "Attributes.Vital.Health", "Health");
	AddTag(Attributes_Vital_Mana, "Attributes.Vital.Mana", "Mana");
	
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

	AddTag(Attributes_Meta_IncomingXP, "Attributes.Meta.IncomingXP", "IncomingXP Meta Attribute");
	
	// Input
	AddTag(InputTag_LMB, "InputTag.LMB", "Input Tag for Left Mouse Button");
	AddTag(InputTag_RMB, "InputTag.RMB", "Input Tag for Right Mouse Button");
	AddTag(InputTag_1, "InputTag.1", "Input Tag for Number 1 Key");
	AddTag(InputTag_2, "InputTag.2", "Input Tag for Number 2 Key");
	AddTag(InputTag_3, "InputTag.3", "Input Tag for Number 3 Key");
	AddTag(InputTag_4, "InputTag.4", "Input Tag for Number 4 Key");
	AddTag(InputTag_Passive_1, "InputTag.Passive.1", "Input Tag for Passive Ability 1");
	AddTag(InputTag_Passive_2, "InputTag.Passive.2", "Input Tag for Passive Ability 2");

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

	// Debuff
	AddTag(Debuff_Burn, "Debuff.Burn", "Debuff for Fire Damage");
	AddTag(Debuff_Stun, "Debuff.Stun", "Debuff for Lightning Damage");
	AddTag(Debuff_Arcane, "Debuff.Arcane", "Debuff for Arcane Damage");
	AddTag(Debuff_Physical, "Debuff.Physical", "Debuff for Physical Damage");

	DamageTypesToDebuffs.Emplace(Damage_Fire, Debuff_Burn);
	DamageTypesToDebuffs.Emplace(Damage_Lightning, Debuff_Stun);
	DamageTypesToDebuffs.Emplace(Damage_Arcane, Debuff_Arcane);
	DamageTypesToDebuffs.Emplace(Damage_Physical, Debuff_Physical);

	AddTag(Debuff_Chance, "Debuff.Chance", "Debuff Chance");
	AddTag(Debuff_Damage, "Debuff.Damage", "Debuff Damage");
	AddTag(Debuff_Duration, "Debuff.Duration", "Debuff Duration");
	AddTag(Debuff_Frequency, "Debuff.Frequency", "Debuff Frequency");
	
	// Effects
	AddTag(Effects_HitReact, "Effects.HitReact", "Enemies React to Hit");

	// Offensive Abilities
	AddTag(Abilities_None, "Abilities.None", "None Ability Tag");
	AddTag(Abilities_Attack, "Abilities.Attack", "Attack Ability Tag");
	AddTag(Abilities_Summon, "Abilities.Summon", "Summon Ability Tag");
	AddTag(Abilities_HitReact, "Abilities.HitReact", "HitReact Ability Tag");
	
	AddTag(Abilities_Fire_FireBolt, "Abilities.Fire.FireBolt", "FireBolt Ability Tag");
	AddTag(Abilities_Fire_FireBlast, "Abilities.Fire.FireBlast", "FireBlast Ability Tag");
	
	AddTag(Abilities_Lightning_Electrocute, "Abilities.Lightning.Electrocute", "Electrocute Ability Tag");
	
	AddTag(Abilities_Arcane_ArcaneShards, "Abilities.Arcane.ArcaneShards", "Arcane Shards Ability Tag");
	
	// Passive Abilities
	AddTag(Abilities_Passive_LifeSiphon, "Abilities.Passive.LifeSiphon", "Life Siphon");
	AddTag(Abilities_Passive_ManaSiphon, "Abilities.Passive.ManaSiphon", "Mana Siphon");
	AddTag(Abilities_Passive_HaloOfProtection, "Abilities.Passive.HaloOfProtection", "Halo Of Protection");

	PassiveAbilityToAttributes.Emplace(Abilities_Passive_LifeSiphon, Attributes_Vital_Health);
	PassiveAbilityToAttributes.Emplace(Abilities_Passive_ManaSiphon, Attributes_Vital_Mana);
	
	// Abilities Cooldown
	AddTag(Cooldown_Fire_FireBolt, "Cooldown.Fire.FireBolt", "FireBolt Cooldown Tag");
	AddTag(Cooldown_Lightning_Electrocute, "Cooldown.Lightning.Electrocute", "Electrocute Cooldown Tag");
	
	// Ability Status and Type
	AddTag(Abilities_Status_Locked, "Abilities.Status.Locked", "Locked Status");
	AddTag(Abilities_Status_Eligible, "Abilities.Status.Eligible", "Gray Ability Icon, can't Equip");
	AddTag(Abilities_Status_Unlocked, "Abilities.Status.Unlocked", "Vivid Ability Icon, available for Equip");
	AddTag(Abilities_Status_Equipped, "Abilities.Status.Equipped", "Equipped Status");
	
	AddTag(Abilities_Type_Offensive, "Abilities.Type.Offensive", "AbilityType OffensiveTag");
	AddTag(Abilities_Type_Passive, "Abilities.Type.Passive", "AbilityType Passive Tag");
	AddTag(Abilities_Type_None, "Abilities.Type.None", "AbilityType None Tag");	

	// Combat Socket
	AddTag(CombatSocket_Weapon, "CombatSocket.Weapon", "Weapon");
	AddTag(CombatSocket_LeftHand, "CombatSocket.LeftHand", "LeftHand");
	AddTag(CombatSocket_RightHand, "CombatSocket.RightHand", "RightHand");
	AddTag(CombatSocket_Tail, "CombatSocket.Tail", "Tail");

	// Montage
	AddTag(Montage_Attack_1, "Montage.Attack.1", "Attack Type 1");
	AddTag(Montage_Attack_2, "Montage.Attack.2", "Attack Type 2");
	AddTag(Montage_Attack_3, "Montage.Attack.3", "Attack Type 3");
	AddTag(Montage_Attack_4, "Montage.Attack.4", "Attack Type 4");

	// Player Block
	AddTag(Player_Block_CursorTrace, "Player.Block.CursorTrace", "Block Tracing Under Cursor");
	AddTag(Player_Block_InputHeld, "Player.Block.InputHeld", "Block Input Held Callback");
	AddTag(Player_Block_InputPressed, "Player.Block.InputPressed", "Block Input Pressed Callback");
	AddTag(Player_Block_InputReleased, "Player.Block.InputReleased", "Block Input Released Callback");

	// Gameplay Cue
	AddTag(GameplayCue_FireBlast, "GameplayCue.FireBlast", "GameplayCue FireBlast");
}

void FAuraGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

