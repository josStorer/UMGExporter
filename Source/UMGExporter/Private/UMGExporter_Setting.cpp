#include "UMGExporter_Setting.h"

UUMGExporter_Setting::UUMGExporter_Setting(const FObjectInitializer& Obj)
{
	ContentSourcePath.Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	ExportTargetPath.Path = FPaths::ConvertRelativePathToFull(FPaths::GameSourceDir()) + FApp::GetProjectName();
	FPaths::NormalizeDirectoryName(ContentSourcePath.Path);
	FPaths::NormalizeDirectoryName(ExportTargetPath.Path);
}
