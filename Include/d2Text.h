/**************************************************************************************\
** File: d2Text.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for classes related to text drawing
**
\**************************************************************************************/
#pragma once
#include "d2Color.h"
#include "d2Resource.h"
namespace d2d
{
	const int FONT_SIZE = 72;
	//const float FONT_HEIGHT_TO_LINE_HEIGHT_RATIO = 0.6f;
	//const float FONT_PADDING = 0.0f;
	class FontReference : public ResourceReference
	{
	public:
		FontReference(const std::string& fontPath);
		~FontReference();
		FTGLOutlineFont* GetFontPtr() const;
	};
	struct TextStyle
	{
		FontReference* fontRefPtr;
		Color color;
		float size;
	};
}

