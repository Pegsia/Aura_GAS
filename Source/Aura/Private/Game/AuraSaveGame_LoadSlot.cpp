// Copyright QiuXuan


#include "Game/AuraSaveGame_LoadSlot.h"


bool UAuraSaveGame_LoadSlot::HasMap(const FString& InMapName)
{
	for(const FSavedMap& SavedMap : SavedMaps)
	{
		if(SavedMap.MapName == InMapName)
		{
			return true;
		}
	}
	return false;
}

FSavedMap UAuraSaveGame_LoadSlot::GetSavedMapByMapName(const FString& InMapName)
{
	for(const FSavedMap& SavedMap : SavedMaps)
	{
		if(SavedMap.MapName == InMapName)
		{
			return SavedMap;
		}
	}
	return FSavedMap();
}

bool UAuraSaveGame_LoadSlot::ReplaceSavedMapByMapName(const FSavedMap& InSavedMap, const FString& InMapName)
{
	for(FSavedMap& SavedMap : SavedMaps)
	{
		if(SavedMap.MapName == InMapName)
		{
			SavedMap = InSavedMap;
			return true;
		}
	}
	return false;
}
