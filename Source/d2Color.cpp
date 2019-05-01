/**************************************************************************************\
** File: d2Color.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Color class
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Color.h"
#include "d2Utility.h"
#include "d2NumberManip.h" 
namespace d2d
{
	Color::Color()
	{
		*this = COLOR_ZERO;
	}
	Color::Color(float r, float g, float b, float a)
	{
		SetFloat(r, g, b, a);
	}
	Color::Color(int r, int g, int b, int a)
	{
		SetInt(r, g, b, a);
	}
	void Color::SetFloat(float r, float g, float b, float a)
	{
		red = d2d::GetClamped(r, VALID_PERCENT_RANGE);
		green = d2d::GetClamped(g, VALID_PERCENT_RANGE);
		blue = d2d::GetClamped(b, VALID_PERCENT_RANGE);
		alpha = d2d::GetClamped(a, VALID_PERCENT_RANGE);
	}
	void Color::SetInt(int r, int g, int b, int a)
	{
		SetFloat((float)r / 255.0f, 
				 (float)g / 255.0f, 
				 (float)b / 255.0f, 
				 (float)a / 255.0f);
	}
	Color operator+(const Color& left, const Color& right)
	{
		return
		{ d2d::GetClampedHigh(left.red + right.red, 1.0f),
		  d2d::GetClampedHigh(left.green + right.green, 1.0f),
		  d2d::GetClampedHigh(left.blue + right.blue, 1.0f),
		  d2d::GetClampedHigh(left.alpha + right.alpha, 1.0f) };
	}
	Color operator-(const Color& left, const Color& right)
	{
		return
		{ d2d::GetClampedLow(left.red - right.red, 0.0f),
		  d2d::GetClampedLow(left.green - right.green, 0.0f),
		  d2d::GetClampedLow(left.blue - right.blue, 0.0f),
		  d2d::GetClampedLow(left.alpha - right.alpha, 0.0f) };
	}
	Color operator*(float left, const Color& right)
	{
		return
		{ d2d::GetClamped(left * right.red, VALID_PERCENT_RANGE),
		  d2d::GetClamped(left * right.green, VALID_PERCENT_RANGE),
		  d2d::GetClamped(left * right.blue, VALID_PERCENT_RANGE),
		  d2d::GetClamped(left * right.alpha, VALID_PERCENT_RANGE) };
	}
	Color operator*(const Color& left, const Color& right)
	{
		return
		{ d2d::GetClamped(left.red * right.red, VALID_PERCENT_RANGE),
		  d2d::GetClamped(left.green * right.green, VALID_PERCENT_RANGE),
		  d2d::GetClamped(left.blue  * right.blue, VALID_PERCENT_RANGE),
		  d2d::GetClamped(left.alpha * right.alpha, VALID_PERCENT_RANGE) };
	}

	//+----------------\------------------------------------------
	//|	  ColorRange   |
	//\----------------/------------------------------------------
	ColorRange::ColorRange(const Color& min, const Color& max)
	{
		Set(min, max);
	}
	void ColorRange::Set(const Color& min, const Color& max)
	{
		redRange.Set(min.red, max.red);
		greenRange.Set(min.green, max.green);
		blueRange.Set(min.blue, max.blue);
		alphaRange.Set(min.alpha, max.alpha);
	}
	Color ColorRange::Lerp(float percentMax) const
	{
		return 
		  { d2d::Lerp(redRange, percentMax),
			d2d::Lerp(greenRange, percentMax),
			d2d::Lerp(blueRange, percentMax),
			d2d::Lerp(alphaRange, percentMax) };
	}
}
