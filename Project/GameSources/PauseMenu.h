#pragma once
#include "stdafx.h"
#include "SpriteGeneric.h"

namespace basecross
{
	class PauseMenu :public GameObject
	{
		std::vector<shared_ptr<SpriteGeneric>> m_menuButton;

		int m_selectMenu;	//選択中のメニュー
		int m_menuNum;		//メニュー数
		bool m_lock;
	public:
		PauseMenu(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_selectMenu(0),
			m_menuNum(2),
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
	};
}