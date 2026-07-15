/*!
@file ObstacleSpring.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class ObstacleSpring : public GameObject
	{
	public:

		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<Transform> m_transform; // トランスフォームコンポーネント

		// 構築と破棄
		ObstacleSpring(
			const shared_ptr<Stage>& stage,
			const Vec3& Scale,//サイズ
			const Vec3& Rot,//回転
			const Vec3& Position,//位置
			const Vec3& MoveDirection//移動の向き
		) :
			GameObject(stage),
			m_scale(Scale),
			m_rot(Rot),
			m_pos(Position),
			m_moveDirection(MoveDirection)
		{}

		virtual ~ObstacleSpring()
		{
		}

		float m_angle = 0.0f;
		float m_moveSpeed = 1.0f;
		float m_roteto = 1.0f;
		float m_gravity = -9.8f;
		float m_velocity = 0.0f;
		float m_floor = 0.0f;

		bool m_isGround = false;

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		void Roteto(float delta);
		void UpdateGravity(float delta);
		void PushPlayer(std::shared_ptr<Player> player);

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj);
		void OnCollisionExit(std::shared_ptr<GameObject>& obj);
	
	private:
		Vec3 m_scale;
		Vec3 m_rot;
		Vec3 m_pos;
		Vec3 m_moveDirection;

		static const float PUSH_POWER;
		static const float NORMALIZE_EPS;
		static const float LOOP_ANGLE;
		static const float DEAD_LINE;

	};

}
//end basecross
