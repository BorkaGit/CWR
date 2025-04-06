// Fill out your copyright notice in the Description page of Project Settings.


#include "Performance/CWRPerformanceSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/CWRPerformanceStatTypes.h"

UCWRPlatformSpecificRenderingSettings::UCWRPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const UCWRPlatformSpecificRenderingSettings* UCWRPlatformSpecificRenderingSettings::Get()
{
	UCWRPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

UCWRPerformanceSettings::UCWRPerformanceSettings()
{
	PerPlatformSettings.Initialize(UCWRPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FCWRPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (ECWRDisplayablePerformanceStat PerfStat : TEnumRange<ECWRDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}
