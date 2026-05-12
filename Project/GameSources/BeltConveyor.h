/*!
@file BeltConveyor.h
@brief ベルトコンベアー
*/

#pragma once
#include "stdafx.h"
#include "FloorDecision.h"

namespace basecross {

	class Player;
	class Port;

	//--------------------------------------------------------------------------------------
	//	class BeltConveyor : public GameObject;
	//--------------------------------------------------------------------------------------
	class BeltConveyor : public GameObject
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTDXModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;
		std::shared_ptr<CollisionObb> m_coll;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port;

		std::shared_ptr<FloorDecision> m_floorDec;

		Vec3 m_pos = Vec3(2.0f,-0.5f,0.0f);
		Vec3 m_scale = Vec3(1.5f, 0.1f, 5.0f);

		float m_speed = 2.0f;
		float m_limitDist;

		bool m_isMove = false;

	public:
		// 構築と破棄
		BeltConveyor(const shared_ptr<Stage>& stage,
			shared_ptr<Port> port) :
			GameObject(stage),
			m_port(port)
		{
		}
		virtual ~BeltConveyor()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};
}
//end basecross
