#pragma once
#include "stdafx.h"
#include "SpriteGeneric.h"
#include "PauseMenu.h"

namespace basecross
{
	class PauseMenu;

	class OptionMenu :public GameObject
	{
		std::vector<std::shared_ptr<SpriteGeneric>> m_menuLabel;
		std::vector<std::shared_ptr<SpriteGeneric>> m_menuButton;
		std::vector<Vec2> m_menuPosList;
		std::shared_ptr<SpriteGeneric> m_menuCursor;
		std::shared_ptr<PauseMenu> m_pause;

		bool m_isOption = false;
		int m_selectMenu;	//選択中のメニュー
		int m_menuNum;		//メニュー数
		bool m_lock;
		int m_selectIndex = 0;	//選択肢のインデックス

		Vec2 cursorPos = Vec2(0, 0);

		enum OptionState
		{
			None,
			BGM,
			SE
		};
		OptionState m_opState = OptionState::None;

	public:
		OptionMenu(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_selectMenu(1),
			m_menuNum(3),
			m_lock(false)
		{
		}

		virtual ~OptionMenu()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;
		void OnDraw()override;

		void SelectMenu();
		void ToPause();

		void CloseOption();

		void SetOption(bool isOption)
		{
			m_isOption = isOption;
		}

		bool GetOption()
		{
			return m_isOption;
		}
	};
}