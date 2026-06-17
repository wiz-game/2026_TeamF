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

		shared_ptr<Sprite> m_Title;

		//テスト用
		shared_ptr<NumberSprite> m_SelectingSprite;

		// 各ステージの画像スプライト
		std::vector<std::shared_ptr<Sprite>> m_StageSprites; 


		//BGMの音量
		float m_BGMVolume = 0.5f;

		void CreateViewLight(); //ビューの作成
		void RegisterResources();
	public:
		//構築と破棄
		SelectStage();
		virtual ~SelectStage();
		
		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新

		//ステージ選択スプライトの作成
		void SpriteCreation();

		//タイトルコントロール
		void TitleControl();

	};
}
//end basecross

