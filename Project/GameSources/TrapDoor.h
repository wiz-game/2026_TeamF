/*!
@file GoalDoor.h
@brief ゴールの扉
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"

namespace basecross
{
	class TrapDoorAxis;

	class TrapDoor : public GameObject
	{
		std::shared_ptr<PNTStaticDraw> m_draw;
		std::shared_ptr<Transform> m_trans;

		std::shared_ptr<TrapDoorAxis> m_owner;

		Vec3 m_pos;
		Vec3 m_scale;
		Vec3 m_rotate;

		bool m_isOpen;

	public:
		TrapDoor(const std::shared_ptr<Stage> stage,
			shared_ptr<TrapDoorAxis> owner,
			const Vec3& scale,
			const Vec3& rotate) :
			GameObject(stage),
			m_owner(owner),
			m_scale(scale),
			m_rotate(rotate),
			m_pos(Vec3(0.0f, 0.0f, scale.z / 2.0f)),
			m_isOpen(false)
		{
		}

		virtual ~TrapDoor()
		{
		}

		void OnCreate() override;	//初期設定用
		void OnUpdate() override;	//更新用
	};
}