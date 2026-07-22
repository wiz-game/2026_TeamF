/*!
@file GoalDoor.h
@brief ゴールの扉
*/

#pragma once
#include "stdafx.h"
#include "Texture2DrawComp.h"


namespace basecross
{
	class Port;

	enum class DoorSide
	{
		Left,
		Right
	};

	class GoalDoor : public StageObjBase
	{
		std::shared_ptr<Texture2DrawComp> m_draw;
		std::shared_ptr<Transform> m_trans;
		std::shared_ptr<Port> m_port = nullptr;

		Vec3 m_startPos;//移動開始時の座標

		Vec3 m_moveDir;//移動する方向
		bool m_isOpen;
		float m_speed;

		DoorSide m_side;//左右どちらの扉か

	public:
		GoalDoor(
			const std::shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position,
			std::shared_ptr<Port> port,
			DoorSide side
		) :
			StageObjBase(stage,Scale,Rot,Position,L"GoalDoor"),
			m_port(port),
			m_side(side),
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