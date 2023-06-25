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
#include <d2NumberManip.h>
namespace d2d
{
	void Menu::Init()
	{
		m_currentButton = m_startingButton;
		while(!m_buttonsPressed.empty())
			m_buttonsPressed.pop();
	}
	void Menu::AddButton(const MenuButton& button, bool startSelected)
	{
		m_buttonList.push_back(button);
		if(startSelected)
			m_currentButton = m_startingButton = m_buttonList.size() - 1;
	}
	void Menu::ClearButtons()
	{
		m_buttonList.clear();
		Init();
	}
	void Menu::RemoveButton(const std::string& label)
	{
		unsigned i = 0;
		for(auto it = m_buttonList.begin(); it != m_buttonList.end(); it++, i++)
			if(it->label == label)
			{
				m_buttonList.erase(it);
				if(m_currentButton > m_buttonList.size())
					m_currentButton--;
				return;
			}
	}
	void Menu::ReplaceButton(const std::string& oldLabel, const MenuButton& newButton)
	{
		for(auto it = m_buttonList.begin(); it != m_buttonList.end(); it++)
			if(it->label == oldLabel)
			{
				(*it) = newButton;
				return;
			}
	}
	void Menu::SetButtons(const std::vector<MenuButton>& buttonList)
	{
		m_buttonList = buttonList;
	}
	unsigned Menu::GetSelectedButtonIndex() const
	{
		return m_currentButton;
	}
	void Menu::SetSelectedButton(unsigned buttonIndex)
	{
		if(m_buttonList.empty())
			m_currentButton = 0;
		else
		{
			unsigned maxIndex = m_buttonList.size() - 1;
			m_currentButton = GetClamped(buttonIndex, { 0, maxIndex });
		}
	}

	const std::string& Menu::GetTitle() const
	{
		return m_title;
	}
	void Menu::SetTitle(const std::string& title)
	{
		m_title = title;
	}
	void Menu::SetTitleStyle(const TextStyle& style)
	{
		m_titleStyle = style;
	}

	const std::string& Menu::GetSubtitle() const
	{
		return m_subtitle;
	}
	void Menu::SetSubtitle(const std::string& title)
	{
		m_subtitle = title;
	}
	void Menu::SetSubtitleStyle(const TextStyle& style)
	{
		m_subtitleStyle = style;
	}

	void Menu::SetBackgroundColor(const d2d::Color& color)
	{
		m_backgroundColor = color;
	}
	void Menu::Set(const std::string& title, const TextStyle& titleStyle,
		const d2d::Color& backgroundColor,
		const std::vector<MenuButton> buttonList)
	{
		SetTitle(title);
		SetTitleStyle(titleStyle);
		SetBackgroundColor(backgroundColor);
		SetButtons(buttonList);
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
			for(unsigned i = 0; i < m_buttonList.size(); ++i)
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
		if(m_currentButton < m_buttonList.size() - 1)
			++m_currentButton;
	}
	void Menu::PressSelected()
	{
		if(m_currentButton < m_buttonList.size())
			m_buttonsPressed.push(m_currentButton);
	}
	bool Menu::PollPressedButton(std::string& labelOut)
	{
		if(m_buttonsPressed.empty())
			return false;
		else
		{
			labelOut = m_buttonList[m_buttonsPressed.front()].label;
			m_buttonsPressed.pop();
			return true;
		}
	}
	bool Menu::PollPressedButton(MenuButton& buttonOut)
	{
		if(m_buttonsPressed.empty())
			return false;
		else
		{
			buttonOut = m_buttonList[m_buttonsPressed.front()];
			m_buttonsPressed.pop();
			return true;
		}
	}
	void Menu::Draw() const
	{
		// Set camera to screen resolution
		b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
		d2d::Window::SetCameraRect({ b2Vec2_zero, resolution });

		// Draw background
		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();
		d2d::Window::SetColor(m_backgroundColor);
		d2d::Window::DrawRect({ b2Vec2_zero, resolution }, true);

		b2Vec2 titleCenter;
		b2Vec2 subtitleCenter;
		bool drawSubtitle = !m_subtitle.empty();

		// Draw menu buttons
		for(unsigned i = 0; i < m_buttonList.size(); ++i)
		{
			// Draw button with optional highlighting
			d2d::Rect buttonRect;
			GetButtonRect(i, buttonRect);
			buttonRect.lowerBound = { buttonRect.lowerBound.x * resolution.x, buttonRect.lowerBound.y * resolution.y };
			buttonRect.upperBound = { buttonRect.upperBound.x * resolution.x, buttonRect.upperBound.y * resolution.y };
			if(i == m_currentButton)
			{
				d2d::Window::SetColor(m_buttonList[i].style.highlightColor);
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
			d2d::Window::SetColor(m_buttonList[i].style.text.color);
			d2d::Window::DrawString(m_buttonList[i].label, m_buttonList[i].style.text.size * resolution.y,
				m_buttonList[i].style.text.fontRefPtr, { d2d::AlignmentAnchorX::CENTER, AlignmentAnchorY::CENTER });
			d2d::Window::PopMatrix();

			if(i == 0)
			{
				// Title/subtitle: Between first button text and top of screen
				if(drawSubtitle)
				{
					float titleY = d2d::Lerp(buttonTextCenter.y, resolution.y, 0.5f);
					titleCenter.Set(buttonTextCenter.x, titleY);
					float subtitleY = d2d::Lerp(buttonTextCenter.y, resolution.y, 0.25f);
					subtitleCenter.Set(buttonTextCenter.x, subtitleY);
				}
				else
				{
					float titleY = d2d::Lerp(buttonTextCenter.y, resolution.y, 0.5f);
					titleCenter.Set(buttonTextCenter.x, titleY);
				}
			}
		}

		// Draw title
		d2d::Window::PushMatrix();
		d2d::Window::Translate(titleCenter);
		d2d::Window::SetColor(m_titleStyle.color);
		d2d::Window::DrawString(m_title, m_titleStyle.size * resolution.y, m_titleStyle.fontRefPtr,
			{ d2d::AlignmentAnchorX::CENTER, AlignmentAnchorY::CENTER });
		d2d::Window::PopMatrix();

		if(drawSubtitle)
		{
			// Draw subtitle
			d2d::Window::PushMatrix();
			d2d::Window::Translate(subtitleCenter);
			d2d::Window::SetColor(m_subtitleStyle.color);
			d2d::Window::DrawString(m_subtitle, m_subtitleStyle.size * resolution.y, m_subtitleStyle.fontRefPtr,
				{ d2d::AlignmentAnchorX::CENTER, AlignmentAnchorY::CENTER });
			d2d::Window::PopMatrix();
		}
	}
	void Menu::GetButtonTextCenter(unsigned button, b2Vec2& buttonTextCenter) const
	{
		int referenceButton{ (int)m_buttonList.size() / 2 };
		float referenceButtonY;
		if(m_buttonList.size() % 2)
		{
			// If number of buttons is odd, reference button is in the middle
			referenceButtonY = 0.5f;
		}
		else
		{
			// Otherwise, reference button is just below the middle.
			referenceButtonY = 0.5f - 0.5f * MENU_BUTTON_GAP - 0.5f * MENU_BUTTON_SIZE.y;
		}
		int numButtonsBelow{ (int)button - referenceButton };
		float buttonSpacing{ MENU_BUTTON_SIZE.y + MENU_BUTTON_GAP };
		float distanceBelowReference{ (float)numButtonsBelow * buttonSpacing };
		buttonTextCenter.Set(0.5f, referenceButtonY - distanceBelowReference);
	}
	void Menu::GetButtonRect(unsigned button, d2d::Rect& buttonRect) const
	{
		b2Vec2 buttonTextCenter;
		GetButtonTextCenter(button, buttonTextCenter);
		buttonRect.SetCenter(buttonTextCenter + MENU_BUTTON_OFFSET, MENU_BUTTON_SIZE);
	}
}


