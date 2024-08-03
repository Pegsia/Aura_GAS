// Copyright QiuXuan


#include "Actor/AuraEffectActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AuraAbilitySystemComponent.h"
#include "AuraAttributeSet.h"

AAuraEffectActor::AAuraEffectActor()
{
 	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::OnEndOverlap);
}

void AAuraEffectActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TScriptInterface<IAbilitySystemInterface> ASCInterface = OtherActor)
	{
		const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		
		UAuraAttributeSet* MutableAureAttributeSet = const_cast<UAuraAttributeSet*>(AuraAttributeSet);
		MutableAureAttributeSet->SetHealth(MutableAureAttributeSet->GetHealth() + 25.f);
		MutableAureAttributeSet->SetMana(MutableAureAttributeSet->GetMana() + 25.f);
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


