/**************************************************************************************\
** File: d2Hjson.h
** Project:
** Author: David Leksen
** Date:
**
** Source code file for loading/saving HJSON values and files
**
\**************************************************************************************/
#include "d2Hjson.h"
#include "d2Utility.h"
#include "d2NumberManip.h" 
#include "d2Color.h"
#include "hjson.h"
#include <string>
namespace d2d
{
	HjsonValue FileToHJSON(const std::string& filePath)
	{
		std::string fileContentString{ FileToString(filePath) };
		return Hjson::Unmarshal(fileContentString.c_str(), fileContentString.size());
	}
	bool IsNonNull(const HjsonValue& data)
	{
		return (data.type() != HjsonValue::UNDEFINED && data.type() != HjsonValue::HJSON_NULL);
	}
	HjsonValue GetMemberValue(const HjsonValue& data, const std::string& name)
	{
		if(data.type() == HjsonValue::MAP && IsNonNull(data[name]))
			return data[name];
		throw HjsonFailedQueryException{ name };
	}
	std::string GetString(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == HjsonValue::STRING)
			return v;
		throw HjsonFailedQueryException{ name };
	}
	float GetFloat(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == HjsonValue::DOUBLE)
			return static_cast<float>(v);
		throw HjsonFailedQueryException{ name };
	}
	double GetDouble(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == HjsonValue::DOUBLE)
			return static_cast<double>(v);
		throw HjsonFailedQueryException{ name };
	}
	int GetInt(const HjsonValue& data, const std::string& name)
	{
		return GetRoundedDouble(GetDouble(data, name));
	}
	bool GetBool(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == HjsonValue::BOOL)
			return static_cast<bool>(v);
		throw HjsonFailedQueryException{ name };
	}

	HjsonValue GetVectorValue(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == HjsonValue::VECTOR && index < v.size())
			return v[index];
		throw HjsonFailedQueryException{ name };
	}
	std::string GetVectorString(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == HjsonValue::STRING)
			return v;
		throw HjsonFailedQueryException{ name };
	}
	float GetVectorFloat(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == HjsonValue::DOUBLE)
			return static_cast<float>(v);
		throw HjsonFailedQueryException{ name };
	}
	double GetVectorDouble(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == HjsonValue::DOUBLE)
			return static_cast<double>(v);
		throw HjsonFailedQueryException{ name };
	}
	int GetVectorInt(const HjsonValue& data, const std::string& name, unsigned index)
	{
		return GetRoundedDouble(GetVectorDouble(data, name, index));
	}
	bool GetVectorBool(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == HjsonValue::BOOL)
			return v;
		throw HjsonFailedQueryException{ name };
	}
	Color GetColorFloat(const HjsonValue& data, const std::string& name)
	{
		return Color{ 
			GetVectorFloat(data, name, 0),
			GetVectorFloat(data, name, 1),
			GetVectorFloat(data, name, 2),
			GetVectorFloat(data, name, 3) };
	}
	Color GetColorInt(const HjsonValue& data, const std::string& name)
	{
		return Color{ 
			GetVectorInt(data, name, 0),
			GetVectorInt(data, name, 1),
			GetVectorInt(data, name, 2),
			GetVectorInt(data, name, 3) };
	}
}