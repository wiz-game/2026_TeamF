/*!
@file MoveFloor.h
@brief 上下する床
*/

#pragma once
#include "stdafx.h"
#include "FloorDecision.h"

namespace basecross {

	class Player;
	class Port;

	enum class MoveAxis { X, Y, Z };

	struct MoveFloorDesc
	{	//	デフォルトの値
		Vec3 pos = Vec3(0.0f, -0.5f, 5.5f);
		Vec3 scale = Vec3(2.0f, 0.1f, 2.0f);
		MoveAxis axis = MoveAxis::Y;
		float speed = 1.0f;
		float limitDist = 3.0f;
		std::shared_ptr<Port> port = nullptr;
	};

	//--------------------------------------------------------------------------------------
	//	class MoveFloor : public GameObject;
	//--------------------------------------------------------------------------------------
	class MoveFloor : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTDXModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;
		std::shared_ptr<CollisionObb> m_coll;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port;

		std::shared_ptr<FloorDecision> m_floorDec;

		Vec3 m_pos;
		Vec3 m_scale;

		MoveAxis m_moveAxis;//どの軸に動くか
		float m_speed;
		float m_limitDist;

		bool m_isUp = false;

	public:
		// 構築と破棄
		MoveFloor(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos,
			const MoveFloorDesc& desc) :
			StageObjBase(StagePtr, Scale, Rot, Pos),
			m_pos(desc.pos),
			m_scale(desc.scale),
			m_moveAxis(desc.axis),
			m_speed(desc.speed),
			m_limitDist(desc.limitDist),
			m_port(desc.port)
		{
		}
		virtual ~MoveFloor()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};
}
//end basecross
