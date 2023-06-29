/**************************************************************************************\
** File: d2Exception.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for exceptions
**
\**************************************************************************************/
#pragma once
#include "d2Utility.h"
namespace d2d
{
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
}
