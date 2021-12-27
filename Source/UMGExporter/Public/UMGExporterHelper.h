#pragma once

class FUMGExporterHelper
{
public:
	static void GetTemplateContent(FString& Result, bool bHeaderFile);

	static FString TemplateReplace(const FString& Source, const FString& TemplateTag, const FString& NewString);

	static FString BPToCppName(FString Name);

	static FString RegexReplace(const FString& Source, const FString& Regex, const FString& Replacement);
};
