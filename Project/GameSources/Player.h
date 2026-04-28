/*!
@file Player.h
@brief プレイヤー
*/

#pragma once
#include "stdafx.h"
#include "MainCamera.h"
#include "PNTDXModelDraw.h"
#include "InkCloud.h"
#include "FloorDecision.h"

namespace basecross {

class MoveFloor;

	class Player : public GameObject 
	{
		std::weak_ptr<MainCamera> m_camera;
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント

		std::shared_ptr<FloorDecision> m_floorDecision = nullptr;
		std::shared_ptr<InkCloud> m_targetCloud;
		float m_height;
		float m_radius;

		float m_moveSpeed;
		float m_maxSpeed;
		float m_accel;
		Vec3 m_pos;
		Vec3 m_forward;
		Vec3 m_velocity;

		float m_ink;
		float m_inkMax;
		float m_inkDecrease;
		float m_fade;	
		bool m_isDraw;	

		Vec3 m_externalVelocity; 

		float m_gravity;
		bool m_isGround;
		
	public :
		Player(const std::shared_ptr<Stage>& stage) :
			GameObject(stage),
			m_height(1.0f),
			m_radius(0.49f),
			m_moveSpeed(0.0f),
			m_maxSpeed(5.0f),
			m_accel(0.99f),
			m_pos(Vec3(0.0f, 0.5f, 0.0f)),
			m_forward(Vec3(0)),
			m_velocity(Vec3(0)),
			m_ink(0.0f),
			m_inkMax(10.0f),
			m_inkDecrease(0.9f),
			m_isDraw(true),
			m_fade(0.0f),
			m_externalVelocity(Vec3(0)),
			m_gravity(-9.8f),
			m_isGround(false)
		{
		}

		void OnCreate() override;
		void OnUpdate() override;

		void OnMove();
		void DropInk();
		void OnDied();

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExit(std::shared_ptr<GameObject>& obj)override;

		void UpdateMoveFloor(const Vec3& movePos);

		// �ｽO�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽ�ｽ�ｽ�ｽ�ｽZ�ｽ�ｽ�ｽ�ｽﾖ撰ｿｽ
		//void AddExternalMove(const Vec3& move);
		//void SetExternalVelocity(const Vec3& v) { m_externalVelocity = v; }
	};

}
//end basecross

