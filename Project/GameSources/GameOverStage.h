/*!
@file GameOverStage.h
@brief ゲームオーバーステージ
*/

#pragma once
#include "stdafx.h"
#include "JoltManager.h"
#include "Sprite.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゴールステージクラス
	//--------------------------------------------------------------------------------------
	class GameOverStage : public Stage
	{
		JoltManager m_jphManger; // Jolt Physics マネージャー
		void CreateViewLight(); //ビューの作成

		InputHandler<GameOverStage> m_InputHandler; // 入力ハンドラー
		shared_ptr<Sprite> m_sprite;
		shared_ptr<Sprite> m_sprite_Button;

	public:
		//構築と破棄
		GameOverStage() :Stage() {}
		virtual ~GameOverStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		virtual void OnUpdate2() override;
		virtual void OnDraw()override; //描画
		void OnPushA(); // Aボタンが押されたときの処理
		void RegisterResources();
		void ButtonMove();
	};
}
//end basecross

