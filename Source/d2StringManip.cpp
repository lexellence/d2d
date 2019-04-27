/**************************************************************************************\
** File: d2StringManip.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for string manipulation functions
**
\**************************************************************************************/
#include "d2StringManip.h"
#include <string>
#include <boost/algorithm/string.hpp>
namespace d2d
{
	std::string GetUpperCase(const std::string& str)
	{
		return boost::algorithm::to_upper_copy(str);
	}
	std::string GetLowerCase(const std::string& str)
	{
		return boost::algorithm::to_lower_copy(str);
	}
	void ToUpperCase(std::string& str)
	{
		boost::algorithm::to_upper(str);
	}
	void ToLowerCase(std::string& str)
	{
		boost::algorithm::to_lower(str);
	}
}