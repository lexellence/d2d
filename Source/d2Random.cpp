/**************************************************************************************\
** File: d2Random.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for random number functions
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Random.h"
#include "d2Range.h"
#include "d2Rect.h"
namespace d2d
{
	namespace
	{
		std::mt19937 m_rng;
	}
	//+---------------------\-------------------------------------
	//|	  Random numbers	|
	//\---------------------/-------------------------------------
	void SeedRandomNumberGenerator(unsigned newSeed)
	{
		m_rng.seed(newSeed);
	}
	float RandomFloatPercent()
	{
		return RandomFloat(VALID_PERCENT_RANGE);
	}
	float RandomFloat(const Range<float>& range)
	{
		return std::uniform_real_distribution<float>{ range.GetMin(), range.GetMax() }(m_rng);
	}
	int RandomInt(const Range<int>& range)
	{
		return std::uniform_int_distribution<int>(range.GetMin(), range.GetMax())(m_rng);
	}
	bool RandomBool()
	{
		return RandomInt({ 0, 1 });
	}
	b2Vec2 RandomVec2InRect(const Rect& rect)
	{
		return
		{ RandomFloat( {rect.lowerBound.x, rect.upperBound.x} ),
		  RandomFloat( {rect.lowerBound.y, rect.upperBound.y} ) };
	}
	b2Vec2 RandomVec2InRange(const b2Vec2& low, const b2Vec2& high)
	{
		return
		{ RandomFloat( {low.x, high.x} ),
		  RandomFloat( {low.y, high.y} ) };
	}
}
