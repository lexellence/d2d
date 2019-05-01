/**************************************************************************************\
** File: d2Hjson.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for loading/saving HJSON values and files
**
\**************************************************************************************/
#pragma once
#include "d2Color.h"
namespace d2d
{
	using HjsonValue = Hjson::Value;

	HjsonValue FileToHJSON(const std::string& filePath);

	bool IsNonNull(const HjsonValue& data);
	HjsonValue GetMemberValue(const HjsonValue& data, const std::string& name);
	std::string GetString(const HjsonValue& data, const std::string& name);
	float GetFloat(const HjsonValue& data, const std::string& name);
	double GetDouble(const HjsonValue& data, const std::string& name);
	int GetInt(const HjsonValue& data, const std::string& name);
	bool GetBool(const HjsonValue& data, const std::string& name);

	HjsonValue GetVectorValue(const HjsonValue& data, const std::string& name, unsigned index);
	std::string GetVectorString(const HjsonValue& data, const std::string& name, unsigned index);
	float GetVectorFloat(const HjsonValue& data, const std::string& name, unsigned index);
	double GetVectorDouble(const HjsonValue& data, const std::string& name, unsigned index);
	int GetVectorInt(const HjsonValue& data, const std::string& name, unsigned index);
	bool GetVectorBool(const HjsonValue& data, const std::string& name, unsigned index);

	Color GetColorFloat(const HjsonValue& data, const std::string& name);
	Color GetColorInt(const HjsonValue& data, const std::string& name);
}