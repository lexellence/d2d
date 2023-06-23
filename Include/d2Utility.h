/**************************************************************************************\
** File: d2Utility.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for miscellaneous useful things
**
\**************************************************************************************/
#pragma once
#include <iostream>
#include "d2Rect.h"

// Logging
//using d2LogSeverity = boost::log::trivial::severity_level;
//const d2LogSeverity d2LogSeverityTrace{ boost::log::trivial::trace };
//const d2LogSeverity d2LogSeverityDebug{ boost::log::trivial::debug };
//const d2LogSeverity d2LogSeverityInfo{ boost::log::trivial::info };
//const d2LogSeverity d2LogSeverityWarning{ boost::log::trivial::warning };
//const d2LogSeverity d2LogSeverityError{ boost::log::trivial::error };
//const d2LogSeverity d2LogSeverityFatal{ boost::log::trivial::fatal };

using d2LogSeverity = unsigned;
const d2LogSeverity d2LogSeverityTrace{ 0 };
const d2LogSeverity d2LogSeverityDebug{ 1 };
const d2LogSeverity d2LogSeverityInfo{ 2 };
const d2LogSeverity d2LogSeverityWarning{ 3 };
const d2LogSeverity d2LogSeverityError{ 4 };
const d2LogSeverity d2LogSeverityFatal{ 5 };

//#define d2LogTrace		BOOST_LOG_TRIVIAL(trace)
//#define d2LogDebug		BOOST_LOG_TRIVIAL(debug)
//#define d2LogInfo		BOOST_LOG_TRIVIAL(info)
//#define d2LogWarning	BOOST_LOG_TRIVIAL(warning)
//#define d2LogError		BOOST_LOG_TRIVIAL(error)
//#define d2LogFatal		BOOST_LOG_TRIVIAL(fatal)

#define d2LogTrace		std::cout
#define d2LogDebug		std::cout
#define d2LogInfo		std::cout
#define d2LogWarning	std::cout
#define d2LogError		std::cout
#define d2LogFatal		std::cout

// Physics operators
const b2Vec2 operator*(b2Vec2 v1, const b2Vec2& v2);
const b2Transform operator+(b2Transform t1, const b2Transform& t2);
const b2Transform operator*(float s, b2Transform t);

namespace d2d
{
	// Exceptions
	struct Exception : public std::runtime_error
	{
		explicit Exception(const std::string& message)
			: std::runtime_error{ message }
		{
			d2LogError << message;
		}
	};
	struct InitException : public Exception
	{
		using Exception::Exception;
	};
	struct AssertionFailedException : public Exception
	{
		using Exception::Exception;
	};
	struct HjsonFailedQueryException : public Exception
	{
		using Exception::Exception;
	};
	struct NetworkException : public Exception
	{
		using Exception::Exception;
	};

	// Physics
	float CalculateKineticEnergy(b2Body* bodyPtr);

	// Alignment
	enum class AlignmentAnchorX
	{
		LEFT, CENTER, RIGHT
	};
	enum class AlignmentAnchorY
	{
		BOTTOM, CENTER, TOP
	};
	struct AlignmentAnchor
	{
		AlignmentAnchorX x{ AlignmentAnchorX::CENTER };
		AlignmentAnchorY y{ AlignmentAnchorY::CENTER };
	};
	Rect GetAlignedRect(float width, float height, const AlignmentAnchor& anchor);

	// Gamepads
	float AxisToUnit(Sint16 value, float deadZone = 6700.0f, float aliveZone = 2000.0f);

	// Files
	std::string FileToString(const std::string& filePath);
}
