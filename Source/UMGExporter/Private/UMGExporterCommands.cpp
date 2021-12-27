#include "UMGExporterCommands.h"

#include "UMGExporterStyle.h"

#define LOCTEXT_NAMESPACE "FUMGExporterModule"

FUMGExporterCommands::FUMGExporterCommands()
	: TCommands<FUMGExporterCommands>(TEXT("UMGExporter"), NSLOCTEXT("Contexts", "UMGExporter", "UMGExporter Plugin"),
	                                  NAME_None, FUMGExporterStyle::GetStyleSetName())
{
}

void FUMGExporterCommands::RegisterCommands()
{
	UI_COMMAND(ExportAction, "UMG Export", "Generate umg cpp code", EUserInterfaceActionType::Button,
	           FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::G));
}

#undef LOCTEXT_NAMESPACE
