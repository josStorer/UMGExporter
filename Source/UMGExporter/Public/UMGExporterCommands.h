#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FUMGExporterCommands : public TCommands<FUMGExporterCommands>
{
public:
	FUMGExporterCommands();

	TSharedPtr<FUICommandInfo> ExportAction;

public:
	// TCommands<> interface
	virtual void RegisterCommands() override;
};
