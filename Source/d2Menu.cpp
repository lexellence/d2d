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
	void Menu::SetViewRect(const Rect& proportionOfScreenRect)
	{
		m_proportionOfScreenRect = proportionOfScreenRect;
	}
	void Menu::SetBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;
	}
	void Menu::SetTitleFont(FontReference* fontPtr)
	{
		m_titleFontPtr = fontPtr;
	}
	void Menu::SetSubtitleFont(FontReference* fontPtr)
	{
		m_subtitleFontPtr = fontPtr;
	}
	void Menu::SetButtonFont(FontReference* fontPtr)
	{
		m_buttonFontPtr = fontPtr;
	}
	void Menu::SetTitleColor(const Color& color)
	{
		m_titleColor = color;
	}
	void Menu::SetSubtitleColor(const Color& color)
	{
		m_subtitleColor = color;
	}
	void Menu::SetTitleTextSize(float size)
	{
		m_titleTextSize = size;
	}
	void Menu::SetSubtitleTextSize(float size)
	{
		m_subtitleTextSize = size;
	}
	void Menu::SetButtonTextSize(float size)
	{
		m_buttonTextSize = size;
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
	const std::string& Menu::GetSubtitle() const
	{
		return m_subtitle;
	}
	void Menu::SetSubtitle(const std::string& title)
	{
		m_subtitle = title;
	}

	void Menu::PressButton(const std::string& label)
	{
		for(int i=0; i < m_buttonList.size(); i++)
			if(m_buttonList[i].label == label)
			{
				m_currentButton = i;
				PressSelected();
				return;
			}
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
				float currentAxisFactor{ AxisToUnit(event.caxis.value) };
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
				Rect buttonRect;
				GetButtonRect(i, buttonRect);
				b2Vec2 mousePosition;
				if(event.type == SDL_MOUSEMOTION)
					mousePosition = Window::GetMousePositionAsPercentOfView(event.motion.x, event.motion.y, m_proportionOfScreenRect);
				else
					mousePosition = Window::GetMousePositionAsPercentOfView(event.button.x, event.button.y, m_proportionOfScreenRect);
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
		Window::SetViewRect(m_proportionOfScreenRect);
		b2Vec2 resolution{ Window::GetViewSize() };
		Window::SetCameraRect({ b2Vec2_zero, resolution });

		// Draw background
		Window::DisableTextures();
		Window::EnableBlending();
		Window::SetColor(m_backgroundColor);
		Window::DrawRect({ b2Vec2_zero, resolution }, true);

		b2Vec2 titleCenter;
		b2Vec2 subtitleCenter;
		bool drawSubtitle = !m_subtitle.empty();

		// Draw menu buttons
		for(unsigned i = 0; i < m_buttonList.size(); ++i)
		{
			Rect buttonRect;
			GetButtonRect(i, buttonRect);
			buttonRect.lowerBound = { buttonRect.lowerBound.x * resolution.x, buttonRect.lowerBound.y * resolution.y };
			buttonRect.upperBound = { buttonRect.upperBound.x * resolution.x, buttonRect.upperBound.y * resolution.y };

			// Draw button background 
			if(i == m_currentButton)
				Window::SetColor(m_buttonList[i].highlightStyle.backgroundColor);
			else
				Window::SetColor(m_buttonList[i].style.backgroundColor);
			Window::DrawRect(buttonRect, true);

			// Draw button border
			if(i == m_currentButton)
				Window::SetColor(m_buttonList[i].highlightStyle.borderColor);
			else
				Window::SetColor(m_buttonList[i].style.borderColor);
			Window::DrawRect(buttonRect, false);

			// Draw button text
			b2Vec2 buttonTextCenter;
			GetButtonTextCenter(i, buttonTextCenter);
			buttonTextCenter = { buttonTextCenter.x * resolution.x, buttonTextCenter.y * resolution.y };
			Window::PushMatrix();
			Window::Translate(buttonTextCenter);
			if(i == m_currentButton)
				Window::SetColor(m_buttonList[i].highlightStyle.textColor);
			else
				Window::SetColor(m_buttonList[i].style.textColor);
			if(m_buttonFontPtr)
				Window::DrawString(m_buttonList[i].label, m_buttonTextSize * resolution.y,
					*m_buttonFontPtr, { AlignmentAnchorX::CENTER, AlignmentAnchorY::CENTER });
			Window::PopMatrix();

			// Position title/subtitle
			if(i == 0)
			{
				if(drawSubtitle)
				{
					float titleY = Lerp(buttonTextCenter.y, resolution.y,
						MENU_TITLE_POSITION_FROM_BUTTON_TO_TOP_WITH_SUBTITLE);
					titleCenter.Set(buttonTextCenter.x, titleY);
					float subtitleY = Lerp(buttonTextCenter.y, resolution.y,
						MENU_SUBTITLE_POSITION_FROM_BUTTON_TO_TOP);
					subtitleCenter.Set(buttonTextCenter.x, subtitleY);
				}
				else
				{
					float titleY = Lerp(buttonTextCenter.y, resolution.y,
						MENU_TITLE_POSITION_FROM_BUTTON_TO_TOP_NO_SUBTITLE);
					titleCenter.Set(buttonTextCenter.x, titleY);
				}
			}
		}

		// Draw title
		Window::PushMatrix();
		Window::Translate(titleCenter);
		Window::SetColor(m_titleColor);
		if(m_titleFontPtr)
			Window::DrawString(m_title, m_titleTextSize * resolution.y,
				*m_titleFontPtr, { AlignmentAnchorX::CENTER, AlignmentAnchorY::CENTER });
		Window::PopMatrix();

		if(drawSubtitle)
		{
			// Draw subtitle
			Window::PushMatrix();
			Window::Translate(subtitleCenter);
			Window::SetColor(m_subtitleColor);
			if(m_subtitleFontPtr)
				Window::DrawString(m_subtitle, m_subtitleTextSize * resolution.y,
					*m_subtitleFontPtr, { AlignmentAnchorX::CENTER, AlignmentAnchorY::CENTER });
			Window::PopMatrix();
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
	void Menu::GetButtonRect(unsigned button, Rect& buttonRect) const
	{
		b2Vec2 buttonTextCenter;
		GetButtonTextCenter(button, buttonTextCenter);
		buttonRect.SetCenter(buttonTextCenter + MENU_BUTTON_OFFSET, MENU_BUTTON_SIZE);
	}
}


