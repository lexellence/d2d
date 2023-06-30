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
	const b2Vec2 MENU_BUTTON_OFFSET{ 0.0f, 0.008f };
	const b2Vec2 MENU_BUTTON_SIZE{ 0.35f, 0.054f };
	const float MENU_BUTTON_GAP = 0.054f;
	const float MENU_TITLE_POSITION_FROM_BUTTON_TO_TOP_NO_SUBTITLE = 0.5f;
	const float MENU_TITLE_POSITION_FROM_BUTTON_TO_TOP_WITH_SUBTITLE = 0.5f;
	const float MENU_SUBTITLE_POSITION_FROM_BUTTON_TO_TOP = 0.25f;

	struct ButtonStyle
	{
		d2d::Color textColor;
		d2d::Color backgroundColor;
		d2d::Color borderColor;
	};
	struct MenuButton
	{
		std::string label;
		ButtonStyle style;
		ButtonStyle highlightStyle;
		union 
		{
			int i;
			float f;
		} userData[2];
	};
	class Menu
	{
	public:
		void Init();
		void SetViewRect(const Rect& proportionOfScreenRect = { b2Vec2_zero, {1.0f,1.0f} });
		void SetBackgroundColor(const Color& color = {});
		void SetTitleColor(const Color& color);
		void SetSubtitleColor(const Color& color);
		void SetTitleFont(FontReference* fontPtr);
		void SetSubtitleFont(FontReference* fontPtr);
		void SetButtonFont(FontReference* fontPtr);
		void SetTitleTextSize(float size);
		void SetSubtitleTextSize(float size);
		void SetButtonTextSize(float size);

		void AddButton(const MenuButton& button, bool startSelected = false);
		void ClearButtons();
		void RemoveButton(const std::string& label);
		void ReplaceButton(const std::string& oldLabel, const MenuButton& newButton);
		void SetButtons(const std::vector<MenuButton>& buttonList);
		unsigned GetSelectedButtonIndex() const;
		void SetSelectedButton(unsigned buttonIndex);
		const std::string& GetTitle() const;
		void SetTitle(const std::string& title = {});
		const std::string& GetSubtitle() const;
		void SetSubtitle(const std::string& title = {});

		void ProcessEvent(const SDL_Event& event);
		bool PollPressedButton(std::string& labelOut);
		bool PollPressedButton(MenuButton& buttonOut);
		void Draw() const;
	private:
		void SelectPrevious();
		void SelectNext();
		void PressSelected();
		void GetButtonTextCenter(unsigned button, b2Vec2& buttonTextCenter) const;
		void GetButtonRect(unsigned button, Rect& buttonRect) const;

		Rect m_proportionOfScreenRect{ {0.0f,0.0f},{1.0f,1.0f} };
		Color m_backgroundColor;
		std::string m_title;
		std::string m_subtitle;
		FontReference* m_titleFontPtr{};
		FontReference* m_subtitleFontPtr{};
		FontReference* m_buttonFontPtr{};
		Color m_titleColor;
		Color m_subtitleColor;
		float m_titleTextSize;
		float m_subtitleTextSize;
		float m_buttonTextSize;

		std::vector<MenuButton> m_buttonList;
		unsigned m_currentButton{ 0 };
		unsigned m_startingButton{ 0 };
		std::queue<unsigned> m_buttonsPressed;

	};
}
