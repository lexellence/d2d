/**************************************************************************************\
** File: d2NumberManip.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for number manipulation functions
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2NumberManip.h"
#include "d2Range.h"
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
	int GetWrappedInt(int value, const Range<int>& range)
	{
		int range_size = range.GetMax() - range.GetMin() + 1;

		if(range_size == 0)
			return range.GetMax();

		if(value < range.GetMin())
			value += range_size * ((range.GetMin() - value) / range_size + 1);

		return range.GetMin() + (value - range.GetMin()) % range_size;
	}
	void WrapInt(int& value, const Range<int>& range)
	{
		int range_size = range.GetMax() - range.GetMin() + 1;

		if(range_size == 0)
			value = range.GetMax();

		if(value < range.GetMin())
			value += range_size * ((range.GetMin() - value) / range_size + 1);

		value = range.GetMin() + (value - range.GetMin()) % range_size;
	}

	float GetWrappedFloat(float value, const Range<float>& range)
	{
		float t = fmodf(value - range.GetMin(), range.GetMax() - range.GetMin());
		return t < 0 ? t + range.GetMax() : t + range.GetMin();
	}
	void WrapFloat(float& value, const Range<float>& range)
	{
		float t = fmodf(value - range.GetMin(), range.GetMax() - range.GetMin());
		value = t < 0 ? t + range.GetMax() : t + range.GetMin();
	}

	float GetWrappedFloat(float value, float modulus)
	{
		value = fmodf(value, modulus);
		if(value < 0.0f)
			value += modulus;
		return value;
	}
	void WrapFloat(float& value, float modulus)
	{
		value = fmodf(value, modulus);
		if(value < 0.0f)
			value += modulus;
	}

	b2Vec2 GetWrappedVector(const b2Vec2& value, const Rect& rect)
	{
		return
		{ GetWrappedFloat(value.x, Range<float>{rect.lowerBound.x, rect.upperBound.x}),
			GetWrappedFloat(value.y, Range<float>{rect.lowerBound.y, rect.upperBound.y}) };
	}
	void WrapVector(b2Vec2& value, const Rect& rect)
	{
		WrapFloat(value.x, Range<float>{rect.lowerBound.x, rect.upperBound.x});
		WrapFloat(value.y, Range<float>{rect.lowerBound.y, rect.upperBound.y});
	}

	b2Vec2 GetWrappedVector(const b2Vec2& value, const b2Vec2& low, const b2Vec2& high)
	{
		return
		{ GetWrappedFloat(value.x, Range<float>{low.x, high.x}),
			GetWrappedFloat(value.y, Range<float>{low.y, high.y}) };
	}
	void WrapVector(b2Vec2& value, const b2Vec2& low, const b2Vec2& high)
	{
		WrapFloat(value.x, Range<float>{low.x, high.x});
		WrapFloat(value.y, Range<float>{low.y, high.y});
	}

	float GetWrappedRadians(float angle)
	{
		return GetWrappedFloat(angle, TWO_PI);
	}
	void WrapRadians(float& angle)
	{
		WrapFloat(angle, TWO_PI);
	}

	float GetWrappedDegrees(float angleDegrees)
	{
		return GetWrappedFloat(angleDegrees, 360.0f);
	}
	void WrapDegrees(float& angleDegrees)
	{
		WrapFloat(angleDegrees, 360.0f);
	}

	// Rounding
	int GetRoundedFloat(float value)
	{
		return static_cast<int>(value >= 0.0f ? (value + 0.5f) : (value - 0.5f));
	}
	void RoundFloat(float& value)
	{
		value = (float)((int)(value >= 0.0f ? (value + 0.5f) : (value - 0.5f)));
	}

	int GetRoundedDouble(double value)
	{
		return static_cast<int>(value >= 0.0 ? (value + 0.5) : (value - 0.5));
	}
	void RoundDouble(double& value)
	{
		value = (double)((int)(value >= 0.0 ? (value + 0.5) : (value - 0.5)));
	}
}