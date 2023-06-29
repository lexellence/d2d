/**************************************************************************************\
** File: d2Text.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for classes related to text drawing
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Font.h"
#include "d2Exception.h"
#include "d2Window.h"
namespace d2d
{
	Font::Font(const std::string& filePath)
	{
		m_dtxFontPtr = dtx_open_font(filePath.c_str(), DTX_FONT_SIZE);
		std::cout << "Font() filePath=" << filePath << std::endl;
		if(!m_dtxFontPtr)
			throw InitException{"Failed to load font: "s + filePath};
		Window::ShowSimpleMessageBox(MessageBoxType::D2D_INFO, "Font::Font"s, filePath);
	}
	Font::~Font()
	{
		if(m_dtxFontPtr)
			dtx_close_font(m_dtxFontPtr);
	}
	dtx_font* Font::GetDTXFontPtr() const
	{
		return m_dtxFontPtr;
	}

}
