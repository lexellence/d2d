/**************************************************************************************\
** File: d2StringManip.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for string manipulation functions
**
\**************************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
namespace d2d
{
	// Convert the case of all letters in a string
	std::string GetUpperCase(const std::string& str);
	std::string GetLowerCase(const std::string& str);
	void ToUpperCase(std::string& str);
	void ToLowerCase(std::string& str);

	// Convert a string with entries separated by commas into vector of individual values
	template<typename T>
	std::vector<T> StringToVector(const std::string& s)
	{
		std::vector<T> result;
		std::stringstream ss(s);
		std::string item;
		while(std::getline(ss, item, ',')) result.push_back(boost::lexical_cast<T>(item));
		return result;
	}

	// Convert basic data types to a string
	template <typename T> std::string ToString(const T& t)
	{
		return boost::lexical_cast<std::string>(t);
	}
}
