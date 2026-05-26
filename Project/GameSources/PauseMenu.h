#pragma once
#include "stdafx.h"
#include "SpriteGeneric.h"

namespace basecross
{
	class PauseMenu :public GameObject
	{
		std::shared_ptr<SpriteGeneric>m_cancel;

		int m_select;
	public:
		PauseMenu(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_select(0)
		{
		}

		virtual ~PauseMenu()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;
		void OnDraw()override;
	};
}