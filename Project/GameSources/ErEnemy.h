/*!
@file ErEnemy.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class ErEnemy : public BaseEnemy
	{
		std::shared_ptr<InkCloud> m_targetInk;

	public:
		// 構築と破棄
		ErEnemy(const shared_ptr<Stage>& stage) :
			BaseEnemy(stage)
		{
		}
		virtual ~ErEnemy()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	protected :
		virtual void UpdateInkErase() override; // インクの消去
		virtual void UpdatePatrol() override; // 徘徊
	};

}
//end basecross
