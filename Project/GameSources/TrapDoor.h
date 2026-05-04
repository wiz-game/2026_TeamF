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

		std::shared_ptr<GameObject> m_owner;

		Vec3 m_pos;
		Vec3 m_scale;

		bool m_isMove;

	public:
		TrapDoor(const std::shared_ptr<Stage> stage,
			shared_ptr<GameObject> owner,
			const Vec3& scale) :
			GameObject(stage),
			m_owner(owner),
			m_scale(scale),
			m_pos(Vec3(0.0f, 0.0f, scale.z / 2.0f)),
			m_isMove(false)
		{
		}

		virtual ~TrapDoor()
		{
		}

		void OnCreate() override;	//初期設定用
		void OnUpdate() override;	//更新用

		bool GetIsMove() const { return m_isMove; } //トラップドアが動いたかどうかを取得

		void SetIsMove(bool isMove) { m_isMove = isMove; } //トラップドアが動いたかどうかを設定
	};
}