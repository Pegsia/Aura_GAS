// Copyright QiuXuan


#include "AbilitySystem/Data/LootTiersInfo.h"


TArray<FLootItemInfo> ULootTiersInfo::GetSpawnLootItemInfos()
{
	TArray<FLootItemInfo> OutLootItemInfo;
	for(const FLootItemInfo& ItemInfo : LootItemInfos)
	{
		for(int32 i = 0; i < ItemInfo.MaxNumberToSpawn; ++i)
		{
			if(FMath::FRandRange(0.f, 100.f) < ItemInfo.ChanceToSpawn)
			{
				// Emplace方法将形参转为实参类型右值初始化
				// FLootItemInfo Temp((FLootItemInfo&&)ItemInfo);
				OutLootItemInfo.Emplace(ItemInfo);
			}
		}
	}

	return OutLootItemInfo;
}
