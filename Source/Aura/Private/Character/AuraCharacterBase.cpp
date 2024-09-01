// Copyright QiuXuan


#include "AuraCharacterBase.h"
#include "AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitialAbilityActorInfo()
{

}

void AAuraCharacterBase::InitialDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeCharacterDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraCharacterBase::InitAuraStartupAbilities() const
{
	if (!HasAuthority()) return;
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	AuraASC->AddCharacterStartupAbilities(StartupAbilities);
}

void AAuraCharacterBase::CharacterDeath()
{
	// Server
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleCharacterDeath();
}


void AAuraCharacterBase::MulticastHandleCharacterDeath_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	
	bDead = true;
	
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
}

void AAuraCharacterBase::Dissolve()
{
	if(MeshDissolveMaterialInstance)
	{
		UMaterialInstanceDynamic* MeshMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MeshDissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, MeshMaterialInstanceDynamic);
		StartMeshDissolveTimeLine(MeshMaterialInstanceDynamic);
	}
	if(WeaponDissolveMaterialInstance)
	{
		UMaterialInstanceDynamic* WeaponMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, WeaponMaterialInstanceDynamic);
		StartWeaponDissolveTimeLine(WeaponMaterialInstanceDynamic);
	}
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation() const
{
	return BloodEffect;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontageArray;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByMontageTag_Implementation(const FGameplayTag& Tag)
{
	for(FTaggedMontage& TaggedMontage : AttackMontageArray)
	{
		if(TaggedMontage.MontageTag.MatchesTagExact(Tag))
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if(SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if(SocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if(SocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	return FVector();
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
