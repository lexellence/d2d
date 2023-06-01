/**************************************************************************************\
** File: d2Random.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for random number functions
**
\**************************************************************************************/
#pragma once
#include "d2Range.h"
#include "d2Rect.h"
namespace d2d
{
	//+---------------------\-------------------------------------
	//|	  Random numbers	|
	//\---------------------/
	//	If half_open is TRUE, returns a random value in [0f,1f).
	//	If half_open is FALSE, returns a random value in [0f,1f].
	//	The result only has a resolution of RAND_MAX+1 values!
	//------------------------------------------------------------
	void SeedRandomNumberGenerator(unsigned newSeed = SDL_GetPerformanceCounter());
	float RandomFloatPercent();
	float RandomFloat(const Range<float>& range);
	int RandomInt(const Range<int>& range);
	bool RandomBool();

	b2Vec2 RandomVec2InRect(const Rect& rect);
	b2Vec2 RandomVec2InRange(const b2Vec2& low, const b2Vec2& high);
}
