/*!
@file MoveFloor.h
@brief 上下する床
*/

#pragma once
#include "stdafx.h"
#include "FloorDecision.h"
#include "Texture2DrawComp.h"

namespace basecross {
	enum class MoveAxis { X, Y, Z };

	struct MoveFloorDesc
	{	//	デフォルトの値
		MoveAxis axis = MoveAxis::Y;
		float speed = 1.0f;
		float limitDist = 3.0f;
		std::shared_ptr<Port> port = nullptr;
	};

	//--------------------------------------------------------------------------------------
	//	class MoveFloor : public StageObjBase;
	//--------------------------------------------------------------------------------------
	class MoveFloor : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<Texture2DrawComp> m_staticDraw;
		std::shared_ptr<CollisionObb> m_coll;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port;

		std::shared_ptr<FloorDecision> m_floorDec;
		std::shared_ptr<SoundItem> m_MoveSound;

		MoveAxis m_moveAxis;//どの軸に動くか
		float m_speed;
		float m_limitDist;

		bool m_isUp = false;
		float m_stopTimer = 0.0f;
		const float PAUSE_TIME = 0.5f; // 停止時間
		bool isConnect = false;

	public:
		// 構築と破棄
		MoveFloor(
			const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos,
			const MoveFloorDesc& desc
		) :
			StageObjBase(stage, Scale, Rot, Pos,L"MoveFloor"),
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
		virtual void OnDestroy() override;

	};

}
//end basecross
