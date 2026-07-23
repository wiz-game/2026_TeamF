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

		MoveAxis axis = MoveAxis::Y;
		float speed = 0.01f;
		std::shared_ptr<Port> port = nullptr;
	};
	class TrapDoorB : public StageObjBase{
		TrapDoorAxisDesc m_AxisDesc;

		Vec3 m_CurrentPosition;
		Vec3 m_CurrentRotation;

		bool m_IsConnected;
		bool m_IsMoved;

		void ApplyCurrentPosition();
	public:
		TrapDoorB(const shared_ptr<Stage>& ptr, const Vec3& scale, const Vec3& rotation, const Vec3& position, const TrapDoorAxisDesc& desc);
		virtual ~TrapDoorB();

		virtual void OnCreate()override;
		virtual void OnUpdate()override;
	};
	//--------------------------------------------------------------------------------------
	//	class TrapDoorAxis : public GameObject;
	//--------------------------------------------------------------------------------------
	class TrapDoorAxis : public StageObjBase
	{
		std::shared_ptr<Transform> m_trans;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		std::shared_ptr<Port> m_port;

		std::shared_ptr<TrapDoor> m_trapDoor;

		Vec3 m_initialRotation;
		Vec3 m_trapDoorScale;
		Vec3 m_scale;

		Vec3 m_rotateVec;

		MoveAxis m_moveAxis;//どの軸に動くか
		float m_speed;

		bool m_isMove = false; //トラップドアが動いたかどうか
		bool isConnect = false;

	public:
		// 構築と破棄

		TrapDoorAxis(const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position,
			const TrapDoorAxisDesc& desc) :
			StageObjBase(stage, Scale, Rot, Position,L"TrapDoor"),
			m_trapDoorScale(Scale),
			m_initialRotation(Rot),
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

		bool GetIsMove() const { return m_isMove; } //トラップドアが動いたかどうかを取得

	};
}
//end basecross
