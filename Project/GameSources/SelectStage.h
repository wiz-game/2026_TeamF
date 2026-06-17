///*!
//@file GameStage.h
//@brief ゲームステージ
//*/
//
#pragma once
#include "stdafx.h"

namespace basecross {
	class NumberSprite;
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class SelectStage : public Stage
	{
		int m_SelectIndex;
		int m_MaxSelectIndex;

		//テスト用
		shared_ptr<NumberSprite> m_SelectingSprite;
		std::shared_ptr<SoundItem> m_sSelectBGM;	//BGM
		void CreateViewLight(); //ビューの作成
		void RegisterResources();
	public:
		//構築と破棄
		SelectStage();
		virtual ~SelectStage() {}
		
		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
	};
}
//end basecross

