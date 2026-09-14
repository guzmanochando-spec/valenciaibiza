using UnrealBuildTool;
using System.Collections.Generic;

public class ValenciaIbizaEditorTarget : TargetRules
{
	public ValenciaIbizaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ValenciaIbiza");
	}
}
