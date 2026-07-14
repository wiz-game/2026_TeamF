/*!
@file GoalStage.h
@brief ゴールステージ
*/

#pragma once
#include "stdafx.h"
#include "Sprite.h"

namespace basecross {
	class StanpSprite;
	//--------------------------------------------------------------------------------------
	//	ゴールステージクラス
	//--------------------------------------------------------------------------------------
	class GoalStage : public Stage
	{
		void CreateViewLight(); //ビューの作成
		void RegisterResources();

		shared_ptr<Sprite> m_sprite;
		vector<shared_ptr<Sprite>> m_sprite_Buttons;
		shared_ptr<StanpSprite> m_ClearStanp;

		InputHandler<GoalStage> m_InputHandler; // 入力ハンドラー

		int m_ButtonScaleIndex = 0;
		float m_ButtonScaleTimer;//アニメーションしていない時は-1
		float m_ButtonScaleRation;//現在のscale倍率
		float m_BGMVolume;


	public:
		//構築と破棄
		GoalStage() :
			Stage(),
			m_ButtonScaleTimer(-1),
			m_ButtonScaleRation(0.0f),
			m_BGMVolume(0.5f)
		{
		}
		virtual ~GoalStage() {}

		virtual void OnCreate()override; //初期化
		virtual void OnUpdate()override; //更新
		void OnPushA();
		void SpriteMove();
	};

	class StanpSprite : public GameObject {
		wstring m_TexKey;
		Vec3 m_Postion;
		Vec2 m_DefaultScale;
		Vec3 m_Rotation;

		wstring m_SEKey;
		bool m_IsPlyedSE;

		bool m_IsAnimationUpdate;
		float m_AnimaitionFactor;
		float m_AnimationTime;
		float m_MaxScaling;
		float m_MinScaling;
		shared_ptr<Sprite> m_Sprite;
	public:
		StanpSprite(const shared_ptr<Stage>& ptr, const wstring& tex,const Vec3& position,const Vec2& scale,float animationTime,float maxScaling,float minScaling);
		virtual ~StanpSprite(){}

		virtual void OnCreate()override;
		virtual void OnUpdate()override;

		void StartAnimation();
		void SetSE(const wstring& key) { m_SEKey = key; }
	};
}
//end basecross

