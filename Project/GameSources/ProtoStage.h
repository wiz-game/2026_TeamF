/*!
@file GameStage.h
@brief プロトタイプステージ
*/

#pragma once
#include "stdafx.h"
#include "JoltManager.h"
#include "PauseMenu.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	プロトタイプステージクラス
	//--------------------------------------------------------------------------------------
	class ProtoStage : public Stage
	{
		JoltManager m_jphManger; // Jolt Physics マネージャー
		shared_ptr<Player> m_Player;	//プレイヤー
		std::shared_ptr<PauseMenu> m_pauseMenu;
		void CreateViewLight(); //ビューの作成

		bool m_isPause = false;

	public:
		//構築と破棄
		ProtoStage() :Stage() {}
		virtual ~ProtoStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		virtual void OnUpdate2() override;
		virtual void OnDraw()override; //描画

		void Pause(bool isPause);
		bool IsPause()const
		{
			return m_isPause && m_pauseMenu;
		}
	};
}
//end basecross

