/**************************************************************************************\
** File: d2Main.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for startup and shutdown functions
**
\**************************************************************************************/
#pragma once
#include "d2Utility.h"
#include <string>
namespace d2d
{	
	struct GamepadSettings
	{
		std::string	controllerDBFilePath;
		float deadZoneStick;
		float deadZoneTrigger;
		float aliveZoneStick;
		float aliveZoneTrigger;
	};

	void Init(d2LogSeverity minSeverityToLog, const std::string& logOutputFilePath);
	void InitGamepads(const GamepadSettings& gamepadSettings);
	void Shutdown();
}
