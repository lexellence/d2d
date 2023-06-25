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
	const float MENU_BUTTON_GAP{ 0.054f };
	struct ButtonStyle
	{
		TextStyle text;
		d2d::Color color;
		d2d::Color highlightColor;
		d2d::Color borderColor;
	};
	struct MenuButton
	{
		std::string label;
		ButtonStyle style;
		int userData{};
	};
	class Menu
	{
	public:
		void Init();
		void AddButton(const MenuButton& button, bool startSelected = false);
		void ClearButtons();
		void RemoveButton(const std::string& label);
		void ReplaceButton(const std::string& oldLabel, const MenuButton& newButton);
		void SetButtons(const std::vector<MenuButton>& buttonList);
		unsigned GetSelectedButtonIndex() const;
		void SetSelectedButton(unsigned buttonIndex);
		const std::string& GetTitle() const;
		void SetTitle(const std::string& title);
		void SetTitleStyle(const TextStyle& style);
		void SetBackgroundColor(const d2d::Color& color);

		void Set(const std::string& title, const TextStyle& titleStyle,
			const d2d::Color& backgroundColor,
			const std::vector<MenuButton> buttonList);

		void ProcessEvent(const SDL_Event& event);
		bool PollPressedButton(std::string& labelOut);
		bool PollPressedButton(MenuButton& buttonOut);
		void Draw() const;
	private:
		void SelectPrevious();
		void SelectNext();
		void PressSelected();
		void GetButtonTextCenter(unsigned button, b2Vec2& buttonTextCenter) const;
		void GetButtonRect(unsigned button, d2d::Rect& buttonRect) const;

		std::vector<MenuButton> m_buttonList;
		std::string m_title;
		TextStyle m_titleStyle;
		d2d::Color m_backgroundColor;

		unsigned m_currentButton{ 0 };
		unsigned m_startingButton{ 0 };
		std::queue<unsigned> m_buttonsPressed;

	};
}
