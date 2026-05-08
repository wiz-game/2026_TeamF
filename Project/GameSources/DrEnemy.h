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
	};

}
//end basecross
