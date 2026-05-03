/*!
@file TrapDoorAxis.h
@brief トラップドア
*/

#pragma once
#include "stdafx.h"
#include "MoveFloor.h"
#include "TrapDoor.h"

namespace basecross {

	class Port;

	struct TrapDoorAxisDesc
	{	//	デフォルトの値
		Vec3 pos = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 scale = Vec3(2.0f, 0.1f, 2.0f);
		Vec3 initialRotation = Vec3(0.0f, 0.0f, 0.0f);
		MoveAxis axis = MoveAxis::Y;
		float speed = 0.01f;
		std::shared_ptr<Port> port = nullptr;
	};

	//--------------------------------------------------------------------------------------
	//	class TrapDoorAxis : public GameObject;
	//--------------------------------------------------------------------------------------
	class TrapDoorAxis : public GameObject
	{
		std::shared_ptr<Transform> m_trans;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		std::shared_ptr<Port> m_port;

		std::shared_ptr<TrapDoor> m_trapDoor;

		Vec3 m_pos;
		Vec3 m_scale;
		Vec3 m_initialRotation;
		Vec3 m_trapDoorScale;

		Vec3 m_rotateVec;

		MoveAxis m_moveAxis;//どの軸に動くか
		float m_speed;

	public:
		// 構築と破棄
		TrapDoorAxis(const shared_ptr<Stage>& stage,
			const TrapDoorAxisDesc& desc) :
			GameObject(stage),
			m_pos(desc.pos),
			m_trapDoorScale(desc.scale),
			m_initialRotation(desc.initialRotation),
			m_moveAxis(desc.axis),
			m_speed(desc.speed),
			m_port(desc.port),
			m_scale(Vec3(0.1f, 0.1f, 0.1f)),
			m_rotateVec(Vec3(0.0f, 0.0f, 0.0f))
		{
		}
		virtual ~TrapDoorAxis()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};
}
//end basecross
