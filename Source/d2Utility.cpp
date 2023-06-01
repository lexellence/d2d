/**************************************************************************************\
** File: d2Utility.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for miscellaneous useful things
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Utility.h"
#include "d2StringManip.h"

const b2Vec2 operator*(b2Vec2 v1, const b2Vec2& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	return v1;
}
const b2Transform operator+(b2Transform t1, const b2Transform& t2)
{
	t1.p += t2.p;
	t1.q.c += t2.q.c;
	t1.q.s += t2.q.s;
	return t1;
}
const b2Transform operator*(float s, b2Transform t)
{
	t.p *= s;
	t.q.c *= s;
	t.q.s *= s;
	return t;
}

namespace d2d
{
	float CalculateKineticEnergy(b2Body* bodyPtr)
	{
		if(!bodyPtr)
			return 0.0f;
		float linearKineticEnergy{ 0.5f * bodyPtr->GetMass() * bodyPtr->GetLinearVelocity().LengthSquared() };
		float angularKineticEnergy{ 0.5f * bodyPtr->GetInertia() * powf(bodyPtr->GetAngularVelocity(),2) };
		return linearKineticEnergy + angularKineticEnergy;
	}

	//+-------------\---------------------------------------------
	//|	   Input    |
	//\-------------/---------------------------------------------
	float AxisToUnit(Sint16 value, float deadZone, float aliveZone)
	{
		static const float maxNegative{ 32768.0f };
		static const float maxPositive{ 32767.0f };
		static const float inBetweenZoneSizeNegative{ maxNegative - aliveZone - deadZone };
		static const float inBetweenZoneSizePositive{ maxPositive - aliveZone - deadZone };

		float floatVal{ (float)value };
		if(floatVal < deadZone && floatVal > -deadZone)
			return 0.0f;
		if(value < 0)
		{
			if(floatVal < -maxNegative + aliveZone)
				return -1.0f;
			else
			{
				float howFarIntoInBetweenZone{ floatVal + deadZone };
				return howFarIntoInBetweenZone / inBetweenZoneSizeNegative;
			}
		}
		else
		{
			if(floatVal > maxPositive - aliveZone)
				return 1.0f;
			else
			{
				float howFarIntoInBetweenZone{ floatVal - deadZone };
				return howFarIntoInBetweenZone / inBetweenZoneSizePositive;
			}
		}
	}
	std::string FileToString(const std::string& filePath)
	{
		std::ifstream inStream{ filePath };
		if(!inStream)
			return "";
		std::stringstream sstr;
		sstr << inStream.rdbuf();
		return sstr.str();
	}
	int StringToImageInitFlag(std::string extensionString)
	{
		ToLowerCase(extensionString);
		if(extensionString == "jpg")
			return IMG_INIT_JPG;
		else if(extensionString == "png")
			return IMG_INIT_PNG;
		else if(extensionString == "tif")
			return IMG_INIT_TIF;
		else if(extensionString == "webp")
			return IMG_INIT_WEBP;
		else
			return 0;
	}
}
