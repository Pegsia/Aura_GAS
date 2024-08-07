// Copyright QiuXuan


#include "AuraCharacterBase.h"
#include "AbilitySystemComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::InitialPrimaryAttributes()
{
	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponent();
	check(DefaultPrimaryAttribute);

	const FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(DefaultPrimaryAttribute, 1.f, EffectContext);
	TargetASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetASC);
	
}

void AAuraCharacterBase::InitialAbilityActorInfo()
{

}
