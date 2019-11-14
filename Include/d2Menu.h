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
#include "d2Window.h"
namespace d2d
{
	class Menu
	{
	public:
		Menu() = delete;
		Menu(const std::vector<std::string>& buttonNames, const TextStyle& buttonTextStyle,
			 const std::string& title, const TextStyle& titleTextStyle,
			 const Color& buttonColor, const Color& buttonHighlightColor, const Color& buttonBorderColor, const Color& backgroundColor);
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

		std::vector<std::string> m_buttonNames;
		TextStyle m_buttonTextStyle;

		std::string m_titleText;
		TextStyle m_titleStyle;

		d2d::Color m_buttonColor;
		d2d::Color m_buttonHighlightColor;
		d2d::Color m_buttonBorderColor;
		d2d::Color m_backgroundColor;

		unsigned m_currentButton;
		std::queue<unsigned> m_buttonsPressed; 

		const b2Vec2 m_buttonOffset{ 0.0f, 0.008f };
		const b2Vec2 m_buttonSize{ 0.35f, 0.054f };
		const float m_buttonGap{ 0.054f };
	};
}
