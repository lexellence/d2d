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

// Logging
using d2LogSeverity = boost::log::trivial::severity_level;
const d2LogSeverity d2LogSeverityTrace{ boost::log::trivial::trace };
const d2LogSeverity d2LogSeverityDebug{ boost::log::trivial::debug };
const d2LogSeverity d2LogSeverityInfo{ boost::log::trivial::info };
const d2LogSeverity d2LogSeverityWarning{ boost::log::trivial::warning };
const d2LogSeverity d2LogSeverityError{ boost::log::trivial::error };
const d2LogSeverity d2LogSeverityFatal{ boost::log::trivial::fatal };

#define d2LogTrace		BOOST_LOG_TRIVIAL(trace)
#define d2LogDebug		BOOST_LOG_TRIVIAL(debug)
#define d2LogInfo		BOOST_LOG_TRIVIAL(info)
#define d2LogWarning	BOOST_LOG_TRIVIAL(warning)
#define d2LogError		BOOST_LOG_TRIVIAL(error)
#define d2LogFatal		BOOST_LOG_TRIVIAL(fatal)

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
			d2LogFatal << message;
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


	// Physics
	float CalculateKineticEnergy(b2Body* bodyPtr);

	// Alignment
	enum class Alignment
	{
		LEFT_TOP, CENTER_TOP, RIGHT_TOP,
		LEFT_CENTER, CENTERED, RIGHT_CENTER,
		LEFT_BOTTOM, CENTER_BOTTOM, RIGHT_BOTTOM
	};

	// Gamepads
	float AxisToUnit(Sint16 value, float deadZone = 6700.0f, float aliveZone = 2000.0f);

	// Files
	std::string FileToString(const std::string& filePath);
	int StringToImageInitFlag(std::string extensionString);
}
