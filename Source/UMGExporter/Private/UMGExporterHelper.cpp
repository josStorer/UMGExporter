#include "UMGExporterHelper.h"

#include <regex>

#include "Misc/FileHelper.h"

void FUMGExporterHelper::GetTemplateContent(FString& Result, bool bHeaderFile)
{
	FString Type = bHeaderFile ? ".h" : ".cpp";
	FString TemplatePath = FPaths::ProjectPluginsDir()
		+ "UMGExporter/Source/UMGExporter/Private/UMGTemplate" + Type + ".txt";
	FFileHelper::LoadFileToString(Result, *TemplatePath);
}

FString FUMGExporterHelper::TemplateReplace(const FString& Source, const FString& TemplateTag, const FString& NewString)
{
	return Source.Replace(*("{{" + TemplateTag + "}}"), *NewString, ESearchCase::CaseSensitive);
}

FString FUMGExporterHelper::BPToCppName(FString Name)
{
	return Name.Replace(TEXT("WBP_"),TEXT(""), ESearchCase::CaseSensitive)
	           .Replace(TEXT("BP_"),TEXT(""), ESearchCase::CaseSensitive)
	           .Replace(TEXT("_C"),TEXT(""), ESearchCase::CaseSensitive);
}

FString FUMGExporterHelper::RegexReplace(const FString& Source, const FString& Regex, const FString& Replacement)
{
	std::string Result{
		std::regex_replace(TCHAR_TO_UTF8(*Source), std::regex{TCHAR_TO_UTF8(*Regex)}, TCHAR_TO_UTF8(*Replacement))
	};
	return FString{Result.c_str()};
}
