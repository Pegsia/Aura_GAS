// Copyright QiuXuan


#include "AuraCharacterBase.h"
#include "AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "AuraAbilitySystemLibrary.h"
#include "AuraGameplayTags.h"
#include "Debuff/AuraDebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

	BurnDebuffNiagaraComponent = CreateDefaultSubobject<UAuraDebuffNiagaraComponent>("DebuffNiagara");
	BurnDebuffNiagaraComponent->SetupAttachment(GetRootComponent());
	BurnDebuffNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	StunDebuffNiagaraComponent = CreateDefaultSubobject<UAuraDebuffNiagaraComponent>("StunDebuffNiagara");
	StunDebuffNiagaraComponent->SetupAttachment(GetRootComponent());
	StunDebuffNiagaraComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;
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
	AuraASC->AddCharacterStartupPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::CharacterDeath(const FVector& ImpulseVector)
{
	// Server
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleCharacterDeath(ImpulseVector);
}

void AAuraCharacterBase::MulticastHandleCharacterDeath_Implementation(const FVector& ImpulseVector)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	
	bDead = true;
	
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(ImpulseVector * 0.1f,  NAME_None, true);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(ImpulseVector, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	OnDeathDelegate.Broadcast(this);
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

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 TagCount)
{
	bIsStunned = TagCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::OnRep_Stunned()
{
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
	if(SocketTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
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

int32 AAuraCharacterBase::GetMinionCount_Implementation() const
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegisteredSignature& AAuraCharacterBase::GetOnAscRegisteredDelegate()
{
	return OnASCRegisteredDelegate;
}

FOnDeathSignature& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

bool AAuraCharacterBase::GetIsShocked_Implementation() const
{
	return bIsShocked;
}

void AAuraCharacterBase::SetIsShocked_Implementation(bool InIsShocked)
{
	bIsShocked = InIsShocked;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontageArray;
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsShocked);
}