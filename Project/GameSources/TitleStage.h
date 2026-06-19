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
		shared_ptr<Sprite> m_sprite_Button;

		//BGMの音量
		float m_BGMVolume = 0.5f;

		void CreateViewLight(); //ビューの作成
		void RegisterResources();
		std::shared_ptr<SoundItem> m_titleBGM;	//BGM
	
		float m_ButtonScaleTimer;//アニメーションしていない時は-1
		float m_ButtonScaleRation;//現在のscale倍率

	public:
		//構築と破棄
		TitleStage() :
			Stage(),
			m_ButtonScaleTimer(-1),
			m_ButtonScaleRation(0.0f)
		{
		}
		virtual ~TitleStage();
		
		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		void SpriteMove();

	};
}
//end basecross

