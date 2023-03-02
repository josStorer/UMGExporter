#include "UMGExporter.h"

#include "UMGExporterStyle.h"
#include "UMGExporterCommands.h"
#include "ToolMenus.h"
#include "UMGExporterHelper.h"
#include "Blueprint/WidgetTree.h"
#include "Misc/FileHelper.h"
#include "ISettingsModule.h"
#include "UMGExporter_Setting.h"

#define LOCTEXT_NAMESPACE "FUMGExporterModule"
typedef FUMGExporterHelper Helper;

void FUMGExporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FUMGExporterStyle::Initialize();
	FUMGExporterStyle::ReloadTextures();

	FUMGExporterCommands::Register();

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUMGExporterModule::RegisterMenus));

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "UMGExporter",
		                                 LOCTEXT("RuntimeSettingsName", "UMGExporter"),
		                                 LOCTEXT("RuntimeSettingsDescription", "Configure UMGExporter plugin"),
		                                 GetMutableDefault<UUMGExporter_Setting>());
	}
}

void FUMGExporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FUMGExporterStyle::Shutdown();

	FUMGExporterCommands::Unregister();

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "UMGExporter_Settings");
	}
}

void FUMGExporterModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	TArray<FAssetEditorExtender>& AssetEditorToolBarExtenderDelegates =
		FAssetEditorToolkit::GetSharedToolBarExtensibilityManager()->GetExtenderDelegates();
	AssetEditorToolBarExtenderDelegates.Add(FAssetEditorExtender::CreateLambda(
		[this](const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects)
		{
			auto ToolbarExtender = MakeShared<FExtender, ESPMode::NotThreadSafe>();

			if (ContextSensitiveObjects.Num() > 0)
			{
				UObject* CurrentObject = ContextSensitiveObjects[0];
				if (CurrentObject && CurrentObject->IsAsset() && !CurrentObject->IsPendingKill())
				{
					if (auto CurrentWidget = Cast<UWidgetBlueprint>(CurrentObject))
					{
						CommandList->MapAction(
							FUMGExporterCommands::Get().ExportAction,
							FExecuteAction::CreateRaw(this, &FUMGExporterModule::PluginButtonClicked, CurrentWidget),
							FCanExecuteAction());
						ToolbarExtender->AddToolBarExtension("WidgetTools",
						                                     EExtensionHook::After, CommandList,
						                                     FToolBarExtensionDelegate::CreateRaw(
							                                     this, &FUMGExporterModule::AddToolBarExtender));
					}
				}
			}
			return ToolbarExtender;
		}));
}

void FUMGExporterModule::AddToolBarExtender(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FUMGExporterCommands::Get().ExportAction);
}

void FUMGExporterModule::PluginButtonClicked(UWidgetBlueprint* WidgetBlueprint)
{
	FString Message = GenerateBindWidgetCode(WidgetBlueprint);
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "{0}"),
		FText::FromString(Message.IsEmpty() ? "UMG Export Success" : Message));
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

FString FUMGExporterModule::GenerateBindWidgetCode(UWidgetBlueprint* WidgetBlueprint)
{
	FString WidgetBlueprintClassName = Helper::BPToCppName(WidgetBlueprint->GetName());
	FString GenFileWithoutExt = GetGeneratedFilePathWithoutExtension(WidgetBlueprint);
	TArray<FBindWidgetInfo> BindContentArray = GetBindWidgetInfo(WidgetBlueprint);

	FString GeneratedFileText;
	FFileHelper::LoadFileToString(GeneratedFileText, *(GenFileWithoutExt + ".h"));
	{
		GeneratedFileText = Helper::RegexReplace(
			GeneratedFileText,
			// (UNewWidgetBlueprint[\s\S]*?\{[\s\S]*?)(\n\};)
			"(U" + WidgetBlueprintClassName + "[\\s\\S]*?\\{[\\s\\S]*?)(\\n\\};)",
			// If the file was just created, WidgetBlueprintClassName will be {{WidgetClassName}} in template and match fail,
			// so {{InsertContent}} won't be inserted
			"$01{{InsertContent}}$02");

		FString InsertContent;
		for (FBindWidgetInfo BindContent : BindContentArray)
		{
			FString VariableDef = BindContent.TypeName + "* " + BindContent.VariableName + ";";
			if (!GeneratedFileText.Contains(VariableDef))
			{
				InsertContent += "\n\n    UPROPERTY(" + BindContent.PropertySpecifiers + ")\n    ";
				InsertContent += VariableDef;
			}
		}

		FFileHelper::SaveStringToFile(
			Helper::TemplateReplace(
				Helper::TemplateReplace(GeneratedFileText, "WidgetClassName", WidgetBlueprintClassName),
				"InsertContent", InsertContent
			), *(GenFileWithoutExt + ".h"));
	}

	FFileHelper::LoadFileToString(GeneratedFileText, *(GenFileWithoutExt + ".cpp"));
	{
		FFileHelper::SaveStringToFile(
			Helper::TemplateReplace(
				Helper::TemplateReplace(GeneratedFileText, "WidgetClassName", WidgetBlueprintClassName),
				"InsertContent", ""
			), *(GenFileWithoutExt + ".cpp"));
	}

	return "";
}

FString FUMGExporterModule::GetGeneratedFilePathWithoutExtension(UWidgetBlueprint* WidgetBlueprint)
{
	auto Setting = GetMutableDefault<UUMGExporter_Setting>();
	FPaths::NormalizeDirectoryName(Setting->ContentSourcePath.Path);
	FPaths::NormalizeDirectoryName(Setting->ExportTargetPath.Path);
	FPaths::RemoveDuplicateSlashes(Setting->ContentSourcePath.Path);
	FPaths::RemoveDuplicateSlashes(Setting->ExportTargetPath.Path);

	FString GenFileWithoutExt;
	{
		FString ContentSourcePath = Setting->ContentSourcePath.Path + "/";
		FString AssetName = Helper::BPToCppName(WidgetBlueprint->GetName());
		FString AssetPath = FPaths::ConvertRelativePathToFull(
			FPackageName::LongPackageNameToFilename(WidgetBlueprint->GetPathName()));

		int32 LastAssetSeparatorIndex;
		AssetPath.FindLastChar('/', LastAssetSeparatorIndex);
		FString AssetFolder = AssetPath.Left(LastAssetSeparatorIndex);

		FString GeneratedFileFolder = Setting->ExportTargetPath.Path + "/" +
			AssetFolder.Replace(*ContentSourcePath, TEXT(""));
		GenFileWithoutExt = GeneratedFileFolder + "/" + AssetName;
	}

	FString HFile = GenFileWithoutExt + ".h";
	FString CppFile = GenFileWithoutExt + ".cpp";
	if (!FPaths::FileExists(HFile))
	{
		FString HFileContent;
		Helper::GetTemplateContent(HFileContent, true);
		FFileHelper::SaveStringToFile(HFileContent, *HFile);
	}
	if (!FPaths::FileExists(CppFile))
	{
		FString CppFileContent;
		Helper::GetTemplateContent(CppFileContent, false);
		FFileHelper::SaveStringToFile(CppFileContent, *CppFile);
	}
	return GenFileWithoutExt;
}

TArray<FUMGExporterModule::FBindWidgetInfo> FUMGExporterModule::GetBindWidgetInfo(UWidgetBlueprint* WidgetBlueprint)
{
	TArray<FBindWidgetInfo> BindWidgetInfo;
	TArray<UWidgetAnimation*> Animations = WidgetBlueprint->Animations;
	TArray<UWidget*> ChildWidgets;
	WidgetBlueprint->WidgetTree->GetAllWidgets(ChildWidgets);

	for (UWidget* ChildWidget : ChildWidgets)
		if (ChildWidget->bIsVariable)
		{
			UClass* ChildWidgetClass = ChildWidget->GetClass();
			while (ChildWidgetClass->GetName().Contains("_C"))
				ChildWidgetClass = ChildWidgetClass->GetSuperClass();
			BindWidgetInfo.Add(
				FBindWidgetInfo{
					"U" + ChildWidgetClass->GetName(),
					ChildWidget->GetName(),
					"Meta=(BindWidget)"
				});
		}
	for (UWidgetAnimation* Animation : Animations)
		BindWidgetInfo.Add(
			FBindWidgetInfo{
				"U" + Animation->GetClass()->GetName(),
				Animation->GetName(),
				"Meta=(BindWidgetAnim), Transient"
			});

	return BindWidgetInfo;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUMGExporterModule, UMGExporter)
