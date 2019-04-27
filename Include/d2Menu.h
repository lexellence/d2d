/**************************************************************************************\
** File: d2Menu.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Menu class
**
\**************************************************************************************/
#pragma once
#include "d2Color.h"
#include "d2Rect.h"
#include <SDL.h>
#include <Box2D/Box2D.h>
#include <string>
#include <vector>
#include <queue>
namespace d2d
{
	class Menu
	{
	public:
		Menu() = delete;
		Menu(const std::vector<std::string>& buttonTextList, const d2d::Color& buttonTextColor, unsigned buttonFontID, float buttonFontSize,
			 const std::string& title, const d2d::Color& titleColor, unsigned titleFontID, float titleFontSize,
			 const d2d::Color& buttonColor, const d2d::Color& buttonHighlightColor, d2d::Color m_buttonBorderColor, const d2d::Color& backgroundColor);
		void Init();
		void ProcessEvent(const SDL_Event& event);
		bool PollPressedButton(std::string& pressedButton);
		void Draw();
	private:
		void SelectPrevious();
		void SelectNext();
		void PressSelected();
		void GetButtonTextCenter(unsigned button, b2Vec2& buttonTextCenter) const;
		void GetButtonRect(unsigned button, d2d::Rect& buttonRect) const;

		const std::vector<std::string> m_buttonTextList;
		const d2d::Color m_buttonTextColor;
		const unsigned m_buttonFontID;
		const float m_buttonFontSize;

		const std::string m_title;
		const d2d::Color m_titleColor;
		const unsigned m_titleFontID;
		const float m_titleFontSize;
		
		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_backgroundColor{ d2d::COLOR_ZERO };

		unsigned m_currentButton;
		std::queue<unsigned> m_buttonsPressed; 

		const b2Vec2 m_buttonOffset{ 0.0f, 0.008f };
		const b2Vec2 m_buttonSize{ 0.35f, 0.054f };
		const float m_buttonGap{ 0.054f };
	};
}
