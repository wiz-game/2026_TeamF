/*!
@file GoalDoor.h
@brief ゴールの扉
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"


namespace basecross
{
	class Port;

	class GoalDoor : public GameObject
	{
		std::shared_ptr<PNTStaticDraw> m_draw;
		std::shared_ptr<Transform> m_trans;
		std::shared_ptr<Port> m_port = nullptr;

		Vec3 m_pos;
		Vec3 m_startPos;//移動開始時の座標
		Vec3 m_scale;

		Vec3 m_moveDir;//移動する方向
		bool m_isOpen;
		float m_speed;

	public:
		GoalDoor(const std::shared_ptr<Stage> stage,
			const Vec3& pos,
			std::shared_ptr<Port> port,
			const Vec3& moveDir) :
			GameObject(stage),
			m_pos(pos),
			m_port(port),
			m_moveDir(moveDir),
			m_scale(Vec3(4.0f,5.0f,1.0f)),
			m_speed(0.05f),
			m_startPos(m_pos)
		{
		}

		virtual ~GoalDoor()
		{
		}

		void OnCreate() override;	//初期設定用
		void OnUpdate() override;	//更新用
	};
}