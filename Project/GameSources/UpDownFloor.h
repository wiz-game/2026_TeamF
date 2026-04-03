/*!
@file UpDownFloor.h
@brief 上下する床
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	class UpDownFloor : public GameObject;
	//--------------------------------------------------------------------------------------
	class UpDownFloor : public GameObject
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTDXModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;
		std::shared_ptr<CollisionObb> m_coll;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port;

		Vec3 m_pos = Vec3(0.0f, 0.0f, 5.0f);
		Vec3 m_scale = Vec3(2.0f, 0.2f, 2.0f);

		float m_moveSpeed;

	public:
		// 構築と破棄
		UpDownFloor(const shared_ptr<Stage>& stage) :
			GameObject(stage),
			m_moveSpeed(1.0f)
		{
		}
		virtual ~UpDownFloor()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
