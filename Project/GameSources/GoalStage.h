/*!
@file GoalStage.h
@brief ゴールステージ
*/

#pragma once
#include "stdafx.h"
#include "Sprite.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゴールステージクラス
	//--------------------------------------------------------------------------------------
	class GoalStage : public Stage
	{
		void CreateViewLight(); //ビューの作成
		void RegisterResources();

		shared_ptr<Sprite> m_sprite;
		shared_ptr<Sprite> m_sprite_Button;

		InputHandler<GoalStage> m_InputHandler; // 入力ハンドラー

		float m_ButtonScaleTimer;//アニメーションしていない時は-1
		float m_ButtonScaleRation;//現在のscale倍率


	public:
		//構築と破棄
		GoalStage() :
			Stage(),
			m_ButtonScaleTimer(-1),
			m_ButtonScaleRation(0.0f)
		{
		}
		virtual ~GoalStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		void OnPushA();
		void SpriteMove();
	};
}
//end basecross

