/**************************************************************************************\
** File: d2NumberManip.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for number manipulation functions
**
\**************************************************************************************/
#pragma once
#include "d2Range.h"
#include "d2Rect.h"
#include <Box2D/Box2D.h>
namespace d2d
{
	// Angle constants
	const float PI_OVER_SIX{ 0.5235988f };
	const float PI_OVER_FOUR{ 0.7853982f };
	const float PI_OVER_THREE{ 1.047198f };
	const float PI_OVER_TWO{ 1.570796f };
	const float TWO_PI_OVER_THREE{ 2.094395f };
	const float THREE_PI_OVER_FOUR{ 2.356194f };
	const float FIVE_PI_OVER_SIX{ 2.617994f };
	const float PI{ 3.141593f };
	const float SEVEN_PI_OVER_SIX{ 3.665191f };
	const float FIVE_PI_OVER_FOUR{ 3.926991f };
	const float FOUR_PI_OVER_THREE{ 4.188790f };
	const float THREE_PI_OVER_TWO{ 4.712389f };
	const float FIVE_PI_OVER_THREE{ 5.235988f };
	const float SEVEN_PI_OVER_FOUR{ 5.497787f };
	const float ELEVEN_PI_OVER_SIX{ 5.759587f };
	const float TWO_PI{ 6.283185f };
	const float RADIANS_PER_DEGREE{ 0.01745329f };
	const float DEGREES_PER_RADIAN{ 57.29578f };

	// Angle unit conversion
	float GetDegreesFromRadians(float radians);
	float GetRadiansFromDegrees(float degrees);

	// Interpolation
	template <typename T> T Lerp(const T& a, const T& b, float percentB)
	{
		return (percentB * b + (1.0f - percentB) * a);
	}
	template <typename T> T Lerp(const Range<T>& range, float percentMax)
	{
		return Lerp(range.GetMin(), range.GetMax(), percentMax);
	}

	// Clamping
	template <typename T> T GetClampedLow(const T& value, const T& min)
	{
		return std::max(value, min);
	}
	template <typename T> T GetClampedHigh(const T& value, const T& max)
	{
		return std::min(value, max);
	}
	template <typename T> void ClampLow(T& value, const T& min)
	{
		if(value < min)
			value = min;
	}
	template <typename T> void ClampHigh(T& value, const T& max)
	{
		if(value > max)
			value = max;
	}
	template <typename T> T GetClamped(const T& value, const Range<T>& range)
	{
		return std::clamp(value, range.GetMin(), range.GetMax());
	}
	template <typename T> void Clamp(T& value, const Range<T>& range)
	{
		ClampLow(value, range.GetMin());
		ClampHigh(value, range.GetMax());
	}
	template <typename T> T GetClampedPercent(const T& value)
	{
		return GetClamped(value, VALID_PERCENT_RANGE);
	}
	template <typename T> void ClampPercent(T& value)
	{
		Clamp(value, VALID_PERCENT_RANGE);
	}
	template <typename T> T GetClampedPercentSigned(const T& value)
	{
		return GetClamped(value, VALID_PERCENT_RANGE_SIGNED);
	}
	template <typename T> void ClampPercentSigned(T& value)
	{
		Clamp(value, VALID_PERCENT_RANGE_SIGNED);
	}

	// Wrapping
	int WrapIntInRange(int value, const Range<int>& range);
	float WrapFloatInRange(float value, const Range<int>& range);
	float WrapFloat(float value, float modulus);

	b2Vec2 WrapVec2InRect(const b2Vec2& value, const Rect& rect);
	b2Vec2 WrapVec2InRange(const b2Vec2& value, const b2Vec2& low, const b2Vec2& high);
	float WrapRadians(float angle);
	float WrapDegrees(float angleDegrees);

	// Rounding
	int GetRoundedFloat(float value);
	int GetRoundedDouble(double value);
}
