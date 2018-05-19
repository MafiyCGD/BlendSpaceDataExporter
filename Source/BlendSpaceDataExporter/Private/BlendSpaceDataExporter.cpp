// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BlendSpaceDataExporter.h"
#include "BlendSpaceDataActions.h"

#define LOCTEXT_NAMESPACE "FBlendSpaceDataExporterModule"

void FBlendSpaceDataExporterModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FBlendSpaceDataActions));
}

void FBlendSpaceDataExporterModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlendSpaceDataExporterModule, BlendSpaceDataExporter)