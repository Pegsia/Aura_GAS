// Copyright QiuXuan


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bLocallyControlled)
	{
		SendMouseTargetData();
	}
	else
	{
		// We are on the server, listen for target data
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		
		// 绑定CallBack
		// /** Maps from an ability spec to the target data. Used to track replicated data and callbacks */
		// AbilityTargetDataMap.FindOrAdd(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, AbilityOriginalPredictionKey))->TargetSetDelegate;
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);
		
		// 尝试收听
		// TSharedPtr<FAbilityReplicatedDataCache> CachedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, AbilityOriginalPredictionKey));
		// CachedData->TargetSetDelegate.Broadcast(CachedData->TargetData, CachedData->ApplicationTag);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData(); // 等待收到广播
		}
	}	
}

void UTargetDataUnderMouse::SendMouseTargetData()
{
	//To be called on server when a new prediction key is received from the client(In an RPC).
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	// Send Target Data to the server
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// 收到了来自客户端的广播，消耗掉存储的TargetData
	// CachedData->TargetData.Clear();
	AbilitySystemComponent.Get()->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
