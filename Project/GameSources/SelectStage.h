///*!
//@file GameStage.h
//@brief ゲームステージ
//*/
//
#pragma once
#include "stdafx.h"

namespace basecross {
	
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class SelectStage : public Stage
	{
		UINT m_SelectIndex;
		UINT m_MaxSelectIndex;
		void CreateViewLight(); //ビューの作成

	public:
		//構築と破棄
		SelectStage();
		virtual ~SelectStage() {}
		
		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
	};
}
//end basecross

