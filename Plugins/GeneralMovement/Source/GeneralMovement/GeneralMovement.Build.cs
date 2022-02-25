// Copyright 2022 Dominik Scherer. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class GeneralMovement : ModuleRules
{
  public GeneralMovement(ReadOnlyTargetRules Target) : base(Target)
  {
    // @attention The logging setup partially depends on the order of macro definitions (which can break with unity builds). Setting the
    // override to a high value will effectively disable unity builds for this module.
    MinSourceFilesForUnityBuildOverride = Int32.MaxValue;

    CppStandard = CppStandardVersion.Cpp17;

    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

    PrivatePCHHeaderFile = "Private/GMC_PCH.h";

    PublicDependencyModuleNames.AddRange(new[]
      { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "SlateCore", "AIModule", "OnlineSubsystem", "UMG" }
    );

    // Public include directories.
    PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Framework"));
    PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Framework/Actors"));
    PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Framework/Components"));
    PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Extras"));
    PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Extras/Steam"));
    PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/Extras/UI"));

    // Private includes directories.
    PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
    PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Framework"));
    PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Framework/Debug"));

    // Uncomment to activate additional check macros throughout the module.
    //PrivateDefinitions.Add("GMC_DO_CHECK_EXTENSIVE");
  }
}
