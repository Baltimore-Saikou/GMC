

using UnrealBuildTool;
using System.Collections.Generic;

public class GMCEditorTarget : TargetRules
{
	public GMCEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "GMC" } );
	}
}
