/**************************************************************************************\
** File: d2Menu.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Menu class
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Menu.h"
#include "d2Utility.h"
#include "d2Window.h"
#include "d2Color.h"
#include "d2Rect.h"
namespace d2d
{
	Menu::Menu(const std::vector<std::string>& buttonNames, const TextStyle& buttonTextStyle,
		const std::string& title, const TextStyle& titleTextStyle,
		const Color& buttonColor, const Color& buttonHighlightColor, const Color& buttonBorderColor, const Color& backgroundColor)
	: m_buttonNames{ buttonNames },
		m_buttonTextStyle{ buttonTextStyle },
		m_titleText{ title },
		m_titleStyle{ titleTextStyle },
		m_buttonColor{ buttonColor },
		m_buttonHighlightColor{ buttonHighlightColor },
		m_buttonBorderColor{ buttonBorderColor },
		m_backgroundColor{ backgroundColor }
	{
		Init();
	}
	void Menu::Init()
	{
		m_currentButton = 0;
		while(!m_buttonsPressed.empty())
			m_buttonsPressed.pop();
	}
	void Menu::ProcessEvent(const SDL_Event& event)
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:	
			switch(event.key.keysym.sym)
			{
			case SDLK_UP:		SelectPrevious();	break;
			case SDLK_DOWN:		SelectNext();		break;
			case SDLK_RETURN:	PressSelected();	break;
			}
			break;
		case SDL_CONTROLLERBUTTONDOWN:	
			switch(event.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_DPAD_UP:		SelectPrevious();	break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:	SelectNext();		break;
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A:			PressSelected();	break;
			}
			break;
		case SDL_CONTROLLERAXISMOTION:	
			if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
			{
				float currentAxisFactor{ d2d::AxisToUnit(event.caxis.value) };
				static float lastAxisFactor{ currentAxisFactor };
				if(lastAxisFactor == 0.0f)
				{
					if(currentAxisFactor > 0.0f)
						SelectNext();
					else if(currentAxisFactor < 0.0f)
						SelectPrevious();
				}
				lastAxisFactor = currentAxisFactor;
			}
			break;
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
			for(std::vector<std::string>::size_type i = 0; i < m_buttonNames.size(); ++i)
			{
				d2d::Rect buttonRect;
				GetButtonRect(i, buttonRect);
				b2Vec2 mousePosition;
				if(event.type == SDL_MOUSEMOTION)
					mousePosition = d2d::Window::GetMousePositionAsPercentOfWindow(event.motion.x, event.motion.y);
				else
					mousePosition = d2d::Window::GetMousePositionAsPercentOfWindow(event.button.x, event.button.y);
				if(buttonRect.Contains(mousePosition))
				{
					m_currentButton = i;
					if(event.type == SDL_MOUSEBUTTONDOWN)
						m_buttonsPressed.push(i);
					break;
				}
			}
			break;
		}
	}
	void Menu::SelectPrevious()
	{
		if(m_currentButton > 0)
			--m_currentButton;
	}
	void Menu::SelectNext()
	{
		if(m_currentButton < m_buttonNames.size() - 1)
			++m_currentButton;
	}
	void Menu::PressSelected()
	{
		m_buttonsPressed.push(m_currentButton);
	}
	bool Menu::PollPressedButton(std::string& buttonText)
	{
		if(m_buttonsPressed.empty())
			return false;
		else
		{
			buttonText = m_buttonNames[m_buttonsPressed.front()];
			m_buttonsPressed.pop();
			return true;
		}
	}
	void Menu::Draw()
	{
		// Set camera to screen resolution
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });

		// Draw background
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();
		d2d::Window::SetColor(m_backgroundColor);
		d2d::Window::DrawRect({ b2Vec2_zero, resolution }, true);

		// Draw menu buttons
		b2Vec2 titleCenter;
		for(unsigned i = 0; i < m_buttonNames.size(); ++i)
		{
			// Draw button with optional highlighting
			d2d::Rect buttonRect;
			GetButtonRect(i, buttonRect);
			buttonRect.lowerBound = { buttonRect.lowerBound.x * resolution.x, buttonRect.lowerBound.y * resolution.y };
			buttonRect.upperBound = { buttonRect.upperBound.x * resolution.x, buttonRect.upperBound.y * resolution.y };
			if(i == m_currentButton)
			{
				d2d::Window::SetColor(m_buttonHighlightColor);
				d2d::Window::DrawRect(buttonRect, true);
			}
			d2d::Window::SetColor({ 0.5f, 0.5f, 0.5f, 0.5f });
			d2d::Window::DrawRect(buttonRect, false);

			// Draw text
			b2Vec2 buttonTextCenter;
			GetButtonTextCenter(i, buttonTextCenter);
			buttonTextCenter = { buttonTextCenter.x * resolution.x, buttonTextCenter.y * resolution.y };
			d2d::Window::PushMatrix();
			d2d::Window::Translate(buttonTextCenter);
			d2d::Window::SetColor(m_buttonTextStyle.color);
			d2d::Window::DrawString(m_buttonNames[i], d2d::Alignment::CENTERED, m_buttonTextStyle.size * resolution.y, m_buttonTextStyle.font);
			d2d::Window::PopMatrix();

			// Save point half-way between first button text and top of screen for title drawing
			if(i == 0)
				titleCenter.Set( buttonTextCenter.x, (buttonTextCenter.y + resolution.y) / 2.0f );
		}

		// Draw title
		d2d::Window::PushMatrix();
		d2d::Window::Translate(titleCenter);
		d2d::Window::SetColor(m_titleStyle.color);
		d2d::Window::DrawString(m_titleText, d2d::Alignment::CENTERED, m_titleStyle.size * resolution.y, m_titleStyle.font);
		d2d::Window::PopMatrix();
	}
	void Menu::GetButtonTextCenter(unsigned button, b2Vec2& buttonTextCenter) const
	{
		if(m_buttonNames.empty())
			return;

		int referenceButton{ (int)m_buttonNames.size() / 2 };
		float referenceButtonY;
		if(m_buttonNames.size() % 2)
		{
			// If number of buttons is odd, reference button is in the middle
			referenceButtonY = 0.5f;
		}
		else
		{
			// Otherwise, reference button is just below the middle.
			referenceButtonY = 0.5f - 0.5f * m_buttonGap - 0.5f * m_buttonSize.y;
		}
		int numButtonsBelow{ (int)button - referenceButton };
		float buttonSpacing{ m_buttonSize.y + m_buttonGap };
		float distanceBelowReference{ (float)numButtonsBelow * buttonSpacing };
		buttonTextCenter.Set(0.5f, referenceButtonY - distanceBelowReference);
	}
	void Menu::GetButtonRect(unsigned button, d2d::Rect& buttonRect) const
	{
		b2Vec2 buttonTextCenter;
		GetButtonTextCenter(button, buttonTextCenter);
		buttonRect.SetCenter(buttonTextCenter + m_buttonOffset, m_buttonSize);
	}
}


