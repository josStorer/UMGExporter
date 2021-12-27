#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "Modules/ModuleManager.h"

class FUMGExporterModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	struct FBindWidgetInfo
	{
		FString TypeName;
		FString VariableName;
		FString PropertySpecifiers;
	};

private:
	TSharedPtr<FUICommandList> PluginCommands;

private:
	void RegisterMenus();

	void AddToolBarExtender(FToolBarBuilder& Builder);

	/** This function will be bound to Command. */
	void PluginButtonClicked(UWidgetBlueprint* WidgetBlueprint);

	FString GenerateBindWidgetCode(UWidgetBlueprint* WidgetBlueprint);

	FString GetGeneratedFilePathWithoutExtension(UWidgetBlueprint* WidgetBlueprint);

	TArray<FBindWidgetInfo> GetBindWidgetInfo(UWidgetBlueprint* WidgetBlueprint);
};
