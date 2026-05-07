/*!
@file BaseEnemy.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class BaseEnemy : public GameObject
	{

		float m_moveSpeed; // 移動速度
		float m_srachRange; // 探索範囲


	public:
		// 構築と破棄
		BaseEnemy(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}
		virtual ~BaseEnemy()
		{
		}
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<Transform> m_transform; // トランスフォームコンポーネント
		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
