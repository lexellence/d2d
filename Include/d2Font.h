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
namespace d2d
{
	const int DTX_FONT_SIZE = 192;
	const float FONT_HEIGHT_TO_LINE_HEIGHT_RATIO = 0.6f;
	const float FONT_PADDING = 0.0f;
    //+--------------------------\--------------------------------
    //|          Font            |
    //\--------------------------/--------------------------------
    class Font
    {
    public:
        explicit Font(const std::string& filePath);
        ~Font();
        dtx_font* GetDTXFontPtr() const;

    private:
        dtx_font* m_dtxFontPtr;
    };

}

