/**************************************************************************************\
** File: d2Color.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Color class
**
\**************************************************************************************/
#pragma once
#include "d2Range.h"
namespace d2d
{
	struct Color
	{
		float red;
		float green;
		float blue;
		float alpha;

		Color();
		Color(float r, float g, float b, float a = 1.0f);
		Color(int r, int g, int b, int a = 255);
		void SetFloat(float r, float g, float b, float a = 1.0f);
		void SetInt(int r, int g, int b, int a = 255);
	};
	Color operator+(const Color& left, const Color& right);
	Color operator-(const Color& left, const Color& right);
	Color operator*(float left, const Color& right);
	Color operator*(const Color& left, const Color& right);

	inline const Color WHITE_OPAQUE{ 1.0f,1.0f,1.0f,1.0f };
	inline const Color COLOR_ZERO{ 0.0f,0.0f,0.0f,0.0f };
	inline const Color BLACK_OPAQUE{ 0.0f,0.0f,0.0f,1.0f };

	struct ColorRange
	{
		Range<float> redRange{};
		Range<float> greenRange{};
		Range<float> blueRange{};
		Range<float> alphaRange{};

		ColorRange() = default;
		ColorRange(const Color& min, const Color& max);
		void Set(const Color& min, const Color& max);
		Color Lerp(float percentMax) const;
	};
}
