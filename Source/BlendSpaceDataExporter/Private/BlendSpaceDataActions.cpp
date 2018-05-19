// Fill out your copyright notice in the Description page of Project Settings.

#include "BlendSpaceDataActions.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"

UClass* FBlendSpaceDataActions::GetSupportedClass() const
{
	return UBlendSpace::StaticClass();
}

uint32 FBlendSpaceDataActions::GetCategories()
{
	return EAssetTypeCategories::Animation;
}

FText FBlendSpaceDataActions::GetName() const
{
	return FText::FromString(TEXT("Blend Space"));
}

FColor FBlendSpaceDataActions::GetTypeColor() const
{
	return FColor(255, 168, 111);
}

void FBlendSpaceDataActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto AnimAssets = GetTypedWeakObjectPtrs<UBlendSpace>(InObjects);

	MenuBuilder.AddMenuEntry(
		FText::FromString("Export BlendSpace Data"),
		FText::FromString("Export BlendSpace Data to file"),
		FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.ViewOptions"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FBlendSpaceDataActions::CustomAssetContext_Clicked, AnimAssets),
			FCanExecuteAction()
		));
}

void FBlendSpaceDataActions::CustomAssetContext_Clicked(TArray<TWeakObjectPtr<UBlendSpace>> Objects)
{
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto BlendSpace = (*ObjIt).Get();
		if (BlendSpace)
		{
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			TSharedPtr<FJsonObject> JsonObjectAxes = MakeShareable(new FJsonObject);
			for (int32 AxisIndex = 0; AxisIndex < 2; ++AxisIndex)
			{
				TSharedPtr<FJsonObject> JsonObjectAxle = MakeShareable(new FJsonObject);
				JsonObjectAxle->SetNumberField("min", BlendSpace->GetBlendParameter(AxisIndex).Min);
				JsonObjectAxle->SetNumberField("max", BlendSpace->GetBlendParameter(AxisIndex).Max);
				JsonObjectAxes->SetObjectField(BlendSpace->GetBlendParameter(AxisIndex).DisplayName, JsonObjectAxle);
			}
			JsonObject->SetObjectField("axes", JsonObjectAxes);

			TSharedPtr<FJsonObject> JsonObjectAnimation = MakeShareable(new FJsonObject);
			for (const FBlendSample& BlendSample : BlendSpace->GetBlendSamples())
			{
				FString position = FString::SanitizeFloat(BlendSample.SampleValue.X) + " " + FString::SanitizeFloat(BlendSample.SampleValue.Y);
				JsonObjectAnimation->SetStringField(BlendSample.Animation->GetName(), position);
			}
			JsonObject->SetObjectField("animations", JsonObjectAnimation);

			FString content;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&content);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			TArray<FString> SaveFilenames;
			FString FileTypes = TEXT("files|*.txt|All files|*.*");
			FString DefaultFilename = BlendSpace->GetName() + ".txt";
			FDesktopPlatformModule::Get()->SaveFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr), "Save as ...",
				FPaths::ProjectDir(), DefaultFilename, FileTypes, EFileDialogFlags::None, SaveFilenames);
			FFileHelper::SaveStringToFile(content, *SaveFilenames.Last());
		}
	}
}