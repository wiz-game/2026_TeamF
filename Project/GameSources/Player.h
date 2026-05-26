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
#include "StageObjBase.h"

namespace basecross {

class MoveFloor;

	class Player : public StageObjBase 
	{
		std::weak_ptr<MainCamera> m_camera;
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント

		std::shared_ptr<FloorDecision> m_floorDecision = nullptr;
		std::shared_ptr<InkCloud> m_targetCloud;

		std::shared_ptr<SoundItem> m_MoveSound;
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

		Player
		(
			const std::shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position,
			const float& InkMax
		);

		virtual ~Player();

		void OnCreate();
		void OnUpdate();
		void OnDestroy();

		void OnMove();
		void DropInk();
		void OnDied();

		void SetInk(float ink)
		{
			m_ink = ink;
		}

		float GetInk()
		{
			return m_ink;
		}

		float GetMaxInk()
		{
			return m_inkMax;
		}

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj);
		void OnCollisionExit(std::shared_ptr<GameObject>& obj);

		void UpdateMoveFloor(const Vec3& movePos);

		// �ｽO�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽ�ｽ�ｽ�ｽ�ｽZ�ｽ�ｽ�ｽ�ｽﾖ撰ｿｽ
		//void AddExternalMove(const Vec3& move);
		//void SetExternalVelocity(const Vec3& v) { m_externalVelocity = v; }
	};

}
//end basecross

