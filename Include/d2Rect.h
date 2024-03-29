/**************************************************************************************\
** File: d2Rect.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the Rect class
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	class Rect
	{
	public:
		b2Vec2 lowerBound;
		b2Vec2 upperBound;

	public:
		Rect() = default;
		Rect(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound);
		void SetBounds(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound);
		void SetCenter(const b2Vec2& center, const b2Vec2& dimensions);
		b2Vec2 GetCenter() const;
		float GetCenterX() const;
		float GetCenterY() const;
		b2Vec2 GetPointAtPercent(const b2Vec2& percent) const;
		b2Vec2 GetDimensions() const;
		float GetWidth() const;
		float GetHeight() const;
		float GetWidthToHeightRatio() const;
		float GetPerimeter() const;
		bool CollidesWith(const Rect& otherRect) const;
		bool Contains(const Rect& otherRect) const;
		bool Contains(const b2Vec2& point, bool upperBoundInclusive = true) const;
		void SortBounds();
	};
	static const Rect UNIT_RECT{ b2Vec2_zero, { 1.0f, 1.0f } };
}
