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
		InputHandler<GoalStage> m_InputHandler; // 入力ハンドラー

	public:
		//構築と破棄
		GoalStage() :Stage() {}
		virtual ~GoalStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		void OnPushA();
	};
}
//end basecross

