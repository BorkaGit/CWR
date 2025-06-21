// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/CustomSettings/CWRSettingValueDiscrete_MobileFPSType.h"

#include "Performance/CWRPerformanceSettings.h"
#include "Settings/CWRSettingsLocal.h"

#define LOCTEXT_NAMESPACE "CWRSettings"

UCWRSettingValueDiscrete_MobileFPSType::UCWRSettingValueDiscrete_MobileFPSType()
{
}

void UCWRSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const UCWRPlatformSpecificRenderingSettings* PlatformSettings = UCWRPlatformSpecificRenderingSettings::Get();
	const UCWRSettingsLocal* UserSettings = UCWRSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (UCWRSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 UCWRSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return UCWRSettingsLocal::GetDefaultMobileFrameRate();
}

FText UCWRSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void UCWRSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void UCWRSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void UCWRSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void UCWRSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 UCWRSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> UCWRSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 UCWRSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return UCWRSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void UCWRSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	UCWRSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE


