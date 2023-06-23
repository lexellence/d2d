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
	const int DTX_FONT_SIZE = 192;
	const float FONT_HEIGHT_TO_LINE_HEIGHT_RATIO = 0.6f;
	const float FONT_PADDING = 0.0f;
	class FontReference : public ResourceReference
	{
	public:
		FontReference(const std::string& fontPath);
		virtual ~FontReference();
		dtx_font* GetDTXFontPtr() const;
	};
	struct TextStyle
	{
		FontReference const* fontRefPtr;
		Color color;
		float size;
	};
}

