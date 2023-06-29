/**************************************************************************************\
** File: d2Hjson.h
** Project:
** Author: David Leksen
** Date:
**
** Source code file for loading/saving HJSON values and files
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Hjson.h"
#include "d2Exception.h"
#include "d2NumberManip.h"
#include "d2Color.h"
namespace d2d
{
	HjsonValue FileToHJSON(const std::string& filePath)
	{
		std::string fileContentString{ FileToString(filePath) };
		return Hjson::Unmarshal(fileContentString.c_str(), fileContentString.size());
	}
	bool IsNonNull(const HjsonValue& data)
	{
		return (data.type() != Hjson::Type::Undefined && data.type() != Hjson::Type::Null);
	}
	HjsonValue GetMemberValue(const HjsonValue& data, const std::string& name)
	{
		if(data.type() == Hjson::Type::Map && IsNonNull(data[name]))
			return data[name];
		throw HjsonFailedQueryException{ name };
	}
	std::string GetString(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == Hjson::Type::String)
			return v;
		throw HjsonFailedQueryException{ name };
	}
	float GetFloat(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == Hjson::Type::Double)
			return static_cast<float>(v);
		throw HjsonFailedQueryException{ name };
	}
	double GetDouble(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == Hjson::Type::Double)
			return static_cast<double>(v);
		throw HjsonFailedQueryException{ name };
	}
	int GetInt(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == Hjson::Type::Int64)
			return static_cast<int>(v);
		throw HjsonFailedQueryException{ name };
	}
	bool GetBool(const HjsonValue& data, const std::string& name)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == Hjson::Type::Bool)
			return static_cast<bool>(v);
		throw HjsonFailedQueryException{ name };
	}

	HjsonValue GetVectorValue(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetMemberValue(data, name) };
		if(v.type() == Hjson::Type::Vector && index < v.size())
			return v[index];
		throw HjsonFailedQueryException{ name };
	}
	std::string GetVectorString(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == Hjson::Type::String)
			return v;
		throw HjsonFailedQueryException{ name };
	}
	float GetVectorFloat(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == Hjson::Type::Double)
			return static_cast<float>(v);
		throw HjsonFailedQueryException{ name };
	}
	double GetVectorDouble(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == Hjson::Type::Double)
			return static_cast<double>(v);
		throw HjsonFailedQueryException{ name };
	}
	int GetVectorInt(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == Hjson::Type::Int64)
			return static_cast<int>(v);
		throw HjsonFailedQueryException{ name };

	}
	bool GetVectorBool(const HjsonValue& data, const std::string& name, unsigned index)
	{
		HjsonValue v{ GetVectorValue(data, name, index) };
		if(v.type() == Hjson::Type::Bool)
			return (bool)v;
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
