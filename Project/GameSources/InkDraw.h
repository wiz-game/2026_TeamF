/*!
@file InkDraw.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class InkDraw : public GameObject
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント

		float m_scaleX = 0.5f; // X軸のスケーリング
		float m_scaleY = 5.0f; // Y軸のスケーリング
	public:
		// 構築と破棄
		InkDraw(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}
		virtual ~InkDraw()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
