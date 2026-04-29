// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyProjectTarget : TargetRules
{
	public MyProjectTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5; 
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;  
        ExtraModuleNames.AddRange( new string[] { "MyProject" } );
	}
}
