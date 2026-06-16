/*!
@file DrEnemy.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class DrEnemy : public BaseEnemy
	{
	public:
		// 構築と破棄
		DrEnemy(const shared_ptr<Stage>& stage) :
			BaseEnemy(stage)
		{
		}
		virtual ~DrEnemy()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	protected :
		virtual void UpdateInkDrow() override; // インクの移動描画
		virtual void UpdatePatrol() override; // 徘徊
	private :
		void SetUpBetWnnePoints();
		bool m_toB = true;

		// インク用
		float m_inkInterval = 0.2f;
		float m_inkTimer = 0.0f;

	};

}
//end basecross
