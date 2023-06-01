/**************************************************************************************\
** File: d2Rect.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Rect class
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Rect.h"
namespace d2d
{
	Rect::Rect(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound)
	  :	lowerBound{ newLowerBound },
		upperBound{ newUpperBound }
	{
		SortBounds();
	}
	void Rect::SetBounds(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound)
	{
		lowerBound = newLowerBound;
		upperBound = newUpperBound;
		SortBounds();
	}
	void Rect::SetCenter(const b2Vec2& center, const b2Vec2& size)
	{
		b2Vec2 halfSize = 0.5f * size;
		lowerBound = center - halfSize;
		upperBound = center + halfSize;
		SortBounds();
	}
	b2Vec2 Rect::GetCenter() const
	{
		return 0.5f * (lowerBound + upperBound);
	}
	float Rect::GetCenterX() const
	{
		return 0.5f * (lowerBound.x + upperBound.x);
	}
	float Rect::GetCenterY() const
	{
		return 0.5f * (lowerBound.y + upperBound.y);
	}
	b2Vec2 Rect::GetPointAtPercent(const b2Vec2& percent) const
	{
		return lowerBound + b2Vec2{ percent.x * GetWidth(), percent.y * GetHeight() };
	}
	b2Vec2 Rect::GetDimensions() const
	{
		return (upperBound - lowerBound);
	}
	float Rect::GetWidth() const
	{
		return upperBound.x - lowerBound.x;
	}
	float Rect::GetHeight() const
	{
		return upperBound.y - lowerBound.y;
	}
	float Rect::GetWidthToHeightRatio() const
	{
		return GetWidth() / GetHeight();
	}
	float Rect::GetPerimeter() const
	{
		return 2.0f * (GetWidth() + GetHeight());
	}
	bool Rect::CollidesWith(const Rect& otherRect) const
	{
		// If we can't find a line that separates the two, then we know they collide.
		return !( lowerBound.x > otherRect.upperBound.x ||
				  otherRect.lowerBound.x > upperBound.x ||
				  lowerBound.y > otherRect.upperBound.y ||
				  otherRect.lowerBound.y > upperBound.y );
	}
	bool Rect::Contains(const Rect& otherRect) const
	{
		return ( lowerBound.x <= otherRect.lowerBound.x &&
				 lowerBound.y <= otherRect.lowerBound.y &&
				 otherRect.upperBound.x <= upperBound.x &&
				 otherRect.upperBound.y <= upperBound.y );
	}
	bool Rect::Contains(const b2Vec2& point, bool upperBoundInclusive) const
	{
		if(point.x >= lowerBound.x &&
			point.x < upperBound.x &&
			point.y >= lowerBound.y &&
			point.y < upperBound.y)
			return true;
		else if(upperBoundInclusive && point == upperBound)
			return true;
		else
			return false;
	}
	void Rect::SortBounds()
	{
		if (lowerBound.x > upperBound.x)
		{
			float temp = lowerBound.x;
			lowerBound.x = upperBound.x;
			upperBound.x = temp;
		}
		if (lowerBound.y > upperBound.y)
		{
			float temp = lowerBound.y;
			lowerBound.y = upperBound.y;
			upperBound.y = temp;
		}
	}
}
