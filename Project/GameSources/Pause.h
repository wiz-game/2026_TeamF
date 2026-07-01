#pragma once
#include "stdafx.h"
#include "PauseMenu.h"
#include "OptionMenu.h"

namespace basecross
{
	class Pause : public GameObject
	{
		std::shared_ptr<PauseMenu> m_pauseMenu;	//ポーズメニュー
		std::shared_ptr<OptionMenu> m_optionMenu;	//設定メニュー

		bool m_isPause = false;

		enum PauseState
		{
			none,
			pause,
			option
		};

	public:
		Pause(const std::shared_ptr<Stage>stage) :
			GameObject(stage)
		{
		}

		virtual ~Pause()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;
		void OnDraw()override;

		void PauseBase();
		void OnPause(bool isPause);
		bool IsPause()const;

		void ToPause();
		void ToOption();
	};
}