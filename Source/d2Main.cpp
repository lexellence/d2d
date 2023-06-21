/**************************************************************************************\
** File: d2Main.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for startup and shutdown functions
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Main.h"
#include "d2Utility.h"
#include "d2Window.h"
#include "d2StringManip.h"
namespace d2d
{
	namespace
	{
		bool m_sdlInitialized{ false };
		bool m_sdlNetInitialized{ false };
		bool OpenJoystickAsGamepad(int joystickDeviceIndex)
		{
			if(SDL_IsGameController(joystickDeviceIndex))
			{
				SDL_GameController* gamepadPtr{ SDL_GameControllerOpen(joystickDeviceIndex) };
				if(gamepadPtr)
				{
					char *mapping{ SDL_GameControllerMapping(gamepadPtr) };
					if(mapping)
					{
						SDL_free(mapping);
						return true;
					}
				}
			}
			return false;
		}
		int d2EventFilter(void* unused, SDL_Event* eventPtr)
		{
			if(eventPtr == nullptr)
				return 0;
			else if(eventPtr->type == SDL_CONTROLLERDEVICEADDED)
			{
				if(d2d::OpenJoystickAsGamepad(eventPtr->cdevice.which))
					d2LogInfo << "Successfully mapped a gamepad.";
				else
					d2LogInfo << "No gamepad mapping found for device index: ";
				return 0;
			}
			return 1;
		}
		SDL_AssertState d2AssertionHandler(const SDL_AssertData* assertPtr, void* userPtr)
		{
			std::stringstream assertionFailedStream;
			assertionFailedStream << "Assertion failed!";
			if(assertPtr)
			{
				assertionFailedStream
					<< " [Condition] "	<< assertPtr->condition
					<< " [File] "		<< assertPtr->filename
					<< " [Function] "	<< assertPtr->function
					<< " [Line] "		<< assertPtr->linenum;
			}
			throw AssertionFailedException{ assertionFailedStream.str() };
		}
	}

	void Init(d2LogSeverity minSeverityToLog, const std::string& logOutputFilePath)
	{
		atexit(d2d::Shutdown);

		// Start logging
		//boost::log::register_simple_formatter_factory<d2LogSeverity, char>("Severity");
		//boost::log::add_file_log
		//(
		//	boost::log::keywords::file_name = logOutputFilePath,
		//	boost::log::keywords::format = "[%Severity%] %Message%"
		//);
		//boost::log::core::get()->set_filter
		//(
		//	boost::log::trivial::severity >= minSeverityToLog
		//);
		//boost::log::add_common_attributes();

		// SDL
		if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::string message{ "SDL_Init failed: " };
			message += SDL_GetError();
			throw InitException{ message };
		}
		m_sdlInitialized = true;

		// SDL_Net
		if(SDLNet_Init() != 0)
		{
			std::string message{ "SDLNet_Init failed: " };
			message += SDLNet_GetError();
			throw InitException{ message };
		}
		m_sdlNetInitialized = true;

		// Set callback/handler funtions
		SDL_SetAssertionHandler(d2AssertionHandler, nullptr);
		SDL_SetEventFilter(d2EventFilter, nullptr);
	}
	void InitGamepads(const GamepadSettings& gamepadSettings)
	{
		// Load controller device mappings
		int mappingsAdded{ SDL_GameControllerAddMappingsFromFile(gamepadSettings.controllerDBFilePath.c_str()) };

		// -1 return value indicates error
		if(mappingsAdded < 0)
		{
			std::string message{ "Could not open controller map database file: "
				+ gamepadSettings.controllerDBFilePath + ": " + SDL_GetError() };
			d2LogError << message;
			d2d::Window::ShowSimpleMessageBox(d2d::MessageBoxType::D2D_WARN, "Error"s, message);
		}

		// Open controllers
		unsigned gamepadsMapped{ 0 };
		unsigned gamepadsFailedToMap{ 0 };
		for(int i = 0; i < SDL_NumJoysticks(); ++i)
		{
			if(OpenJoystickAsGamepad(i))
				++gamepadsMapped;
			else
				++gamepadsFailedToMap;
		}

		std::string gamepadsMessage{ ToString(gamepadsMapped) + " gamepads mapped. " };
		if(gamepadsFailedToMap > 0)
			gamepadsMessage += ", " + ToString(gamepadsFailedToMap) + " gamepads failed to map";
		d2LogInfo << gamepadsMessage;

		// Enable controller events
		SDL_GameControllerEventState(SDL_ENABLE);
	}
	void Shutdown()
	{
		d2d::Window::Close();
		if(m_sdlNetInitialized)
		{
			SDLNet_Quit();
			m_sdlNetInitialized = false;
		}
		if(m_sdlInitialized)
		{
			SDL_Quit();
			m_sdlInitialized = false;
		}
	}
}
