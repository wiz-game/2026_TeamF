///*!
//@file GameStage.h
//@brief ゲームステージ
//*/
//
#pragma once
#include "stdafx.h"
//#include "JoltManager.h"
namespace basecross {
	class Sprite;

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class TitleStage : public Stage
	{
		shared_ptr<Sprite> m_Title;

		//BGMの音量
		float m_BGMVolume = 0.5f;

		void CreateViewLight(); //ビューの作成
		void RegisterResources();
		std::shared_ptr<SoundItem> m_titleBGM;	//BGM
	public:
		//構築と破棄
		TitleStage() :Stage(){}
		virtual ~TitleStage();
		
		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
	};
}
//end basecross

