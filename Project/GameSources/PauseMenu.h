#pragma once
#include "stdafx.h"
#include "SpriteGeneric.h"
#include "OptionMenu.h"

namespace basecross
{
	class PauseMenu :public GameObject
	{
		std::vector<std::shared_ptr<SpriteGeneric>> m_menuLabel;
		std::vector<std::shared_ptr<SpriteGeneric>> m_menuButton;
		std::vector<Vec2> m_menuPosList;
		std::shared_ptr<SpriteGeneric> m_menuCursor;
		std::shared_ptr<OptionMenu> m_option;

		bool m_isPause = false;
		int m_selectMenu;	//選択中のメニュー
		int m_menuNum;		//メニュー数
		bool m_lock;

		int m_selectIndex = 0;	//選択肢のインデックス

		Vec2 cursorPos = Vec2(0, 0);

	public:
		PauseMenu(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_selectMenu(1),
			m_menuNum(5),
			m_lock(false)
		{
		}

		virtual ~PauseMenu()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;
		void OnDraw()override;

		void SelectMenu();
		void ClosePause();
		void ToOption();

		void SetPause(bool isPause)
		{
			m_isPause = isPause;
		}

		bool GetPause()
		{
			return m_isPause;
		}
	};
}