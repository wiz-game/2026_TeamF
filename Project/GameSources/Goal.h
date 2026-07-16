/*!
@file Goal.h
@brief ゴール
*/

#pragma once
#include "stdafx.h"
#include "Sprite.h"

namespace basecross {
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

		std::shared_ptr<Sprite> m_fadeSprite = nullptr;
		std::shared_ptr<SpriteFade> m_fadeComp = nullptr;

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
			StageObjBase(StagePtr, Scale, Rot, Pos,L"Goal"),
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

	//--------------------------------------------------------------------------------------
	//	エフェクト
	//--------------------------------------------------------------------------------------

	class GoalParticle : public GameObject
	{
		std::shared_ptr<Transform> m_trans;
		std::shared_ptr<PNTStaticDraw> m_draw;
		std::shared_ptr<Port> m_port;

		Vec3 m_centerPos;
		float m_radius;
		float m_angle;
		float m_speed;
		Vec3 m_scale;
		float m_alpha;

	public:
		GoalParticle(const std::shared_ptr<Stage>& stage,
			const Vec3& centerPos,const Vec3& scale, 
			float radius, float angle,float alpha, std::shared_ptr<Port> port
		)
			: GameObject(stage),
			m_centerPos(centerPos),
			m_scale(scale),
			m_radius(radius),
			m_angle(angle),
			m_alpha(alpha),
			m_port(port),
			m_speed(5.0f)
		{}
		void OnCreate() override;
		void OnUpdate() override;
	};

	class GoalEffect : public GameObject
	{
		std::shared_ptr<Port> m_port;

		Vec3 m_centerPos;	//中心位置
		float m_radius;		//中心からの距離
		Vec3 m_scale;		//球体の大きさ
		int m_total;		//球体の全ての数

	public:
		GoalEffect(const std::shared_ptr<Stage>& stage,
			const Vec3& centerPos,const Vec3& scale, float radius,  std::shared_ptr<Port> port
		)
			: GameObject(stage),
			m_centerPos(centerPos),
			m_scale(scale),
			m_radius(radius),
			m_port(port),
			m_total(50)
		{}
		void OnCreate() override;
	};
}
//end basecross
