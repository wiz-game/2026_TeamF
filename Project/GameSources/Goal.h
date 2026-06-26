/*!
@file Goal.h
@brief ゴール
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Sprite;
	//--------------------------------------------------------------------------------------
	//	class Goal : public GameObject;
	//--------------------------------------------------------------------------------------
	class Goal : public StageObjBase
	{
		enum class State {
			Wait,			//通電待ち
			Vibrate,		//振動アニメーション
			SpriteScale,	//スプライトアニメーション
			End				//演出終了

		};

		State m_state;

		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTBoneModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port = nullptr;

		std::shared_ptr<Sprite> m_resultSprite;

		std::shared_ptr<SoundItem> m_MoveSound_1;
		std::shared_ptr<SoundItem> m_MoveSound_2;


		Vec3 m_startPos;	   //初期位置
		float m_speed;
		float m_animatimer;	   //共通のタイマー
		Vec2 m_spriteStartPos2D; //スプライトの初期位置

	public:
		// 構築と破棄
		Goal(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos,
			std::shared_ptr<Port> port
		) :
			StageObjBase(StagePtr, Scale, Rot, Pos),
			m_port(port),
			m_startPos(Pos),
			m_speed(1.5f),
			m_animatimer(0.0f),
			m_state(State::Wait)//初期状態は通電待ち
		{
		}
		virtual ~Goal()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
		void RegisterResources();

		void VibrateAnimation(float delta);
		void SpriteAnimation(float delta);
	};

}
//end basecross
