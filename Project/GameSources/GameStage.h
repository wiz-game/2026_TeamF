/*!
@file GameStage.h
@brief ゲームステージ
*/

#pragma once
#include "stdafx.h"
#include "JoltManager.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage
	{
		JoltManager m_jphManger; // Jolt Physics マネージャー

		void CreateViewLight(); //ビューの作成

	public:
		//構築と破棄
		GameStage() :Stage(){}
		virtual ~GameStage() {}
		
		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		virtual void OnUpdate2() override;
		virtual void OnDraw()override; //描画
	};
}
//end basecross

