#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UMGExporter_Setting.generated.h"

/**
 * 
 */
UCLASS(Config=UMGExporterSetting)
class UMGEXPORTER_API UUMGExporter_Setting : public UObject
{
	GENERATED_BODY()
	
public:
	UUMGExporter_Setting(const FObjectInitializer& Obj);

	/** The path of UMG blueprint file relative to this directory is maintained and generated in ExportTargetPath, default is Content*/
	UPROPERTY(Config, EditAnywhere, Category = "UMGExporter Settings")
	FDirectoryPath ContentSourcePath;

	/** Export relative location of the generated file, default is Source/[ProjectName]*/
	UPROPERTY(Config, EditAnywhere, Category = "UMGExporter Settings")
	FDirectoryPath ExportTargetPath;
};
