using UnrealBuildTool;
using System.Collections.Generic;

public class ValenciaIbizaTarget : TargetRules
{
	public ValenciaIbizaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ValenciaIbiza");
	}
}
