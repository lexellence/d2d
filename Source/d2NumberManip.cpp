/**************************************************************************************\
** File: d2NumberManip.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for number manipulation functions
**
\**************************************************************************************/
#include "d2NumberManip.h"
#include "d2Range.h"
#include <Box2D/Box2D.h>
namespace d2d
{
	//+---------------------------\-------------------------------
	//|	  Angle unit conversion   |
	//\---------------------------/-------------------------------
	float GetDegreesFromRadians(float radians)
	{
		return radians * DEGREES_PER_RADIAN;
	}
	float GetRadiansFromDegrees(float degrees)
	{
		return degrees * RADIANS_PER_DEGREE;
	}

	//+-------------\---------------------------------------------
	//|	 Wrapping   |
	//\-------------/---------------------------------------------
	int WrapIntInRange(int value, const Range<int>& range)
	{
		int range_size = range.GetMax() - range.GetMin() + 1;

		if(range_size == 0)
			return range.GetMax();

		if(value < range.GetMin())
			value += range_size * ((range.GetMin() - value) / range_size + 1);

		return range.GetMin() + (value - range.GetMin()) % range_size;
	}
	float WrapFloatInRange(float value, const Range<float>& range)
	{
		float t = fmodf(value - range.GetMin(), range.GetMax() - range.GetMin());
		return t < 0 ? t + range.GetMax() : t + range.GetMin();
	}
	float WrapFloat(float value, float modulus)
	{
		value = fmodf(value, modulus);
		if(value < 0.0f)
			value += modulus;
		return value;
	}
	b2Vec2 WrapVec2InRect(const b2Vec2& value, const Rect& rect)
	{
		return
		{ WrapFloatInRange(value.x, Range<float>{rect.lowerBound.x, rect.upperBound.x}),
			WrapFloatInRange(value.y, Range<float>{rect.lowerBound.y, rect.upperBound.y}) };
	}
	b2Vec2 WrapVec2InRange(const b2Vec2& value, const b2Vec2& low, const b2Vec2& high)
	{
		return
		{ WrapFloatInRange(value.x, Range<float>{low.x, high.x}),
			WrapFloatInRange(value.y, Range<float>{low.y, high.y}) };
	}
	float WrapRadians(float angle)
	{
		return WrapFloat(angle, TWO_PI);
	}
	float WrapDegrees(float angleDegrees)
	{
		return WrapFloat(angleDegrees, 360.0f);
	}

	// Rounding
	int GetRoundedFloat(float value)
	{
		return static_cast<int>(value >= 0.0f ? (value + 0.5f) : (value - 0.5f));
	}
	int GetRoundedDouble(double value)
	{
		return static_cast<int>(value >= 0.0 ? (value + 0.5) : (value - 0.5));
	}
}