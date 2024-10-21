// Copyright QiuXuan

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	// 按下1时，想触发ASC里有InputTag.1的Ability，就需要将InputAction和GameplayTag联系起来
	template<class UserClass, typename PressedFuncType, typename HeldFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, HeldFuncType HeldFunc, ReleasedFuncType ReleasedFunc);
};

//为所有InputAction绑定三种行为，template 中 typename 起到自动推断类型的作用
template<class UserClass, typename PressedFuncType, typename HeldFuncType, typename ReleasedFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, HeldFuncType HeldFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions) 
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}		
	}
}
