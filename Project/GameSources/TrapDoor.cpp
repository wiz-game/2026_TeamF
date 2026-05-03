/*!
@file GoalDoor.cpp
@brief ゴールの扉
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	void TrapDoor::OnCreate()
	{
		//Drawコンポーネント
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetParent(m_owner);
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);
		m_trans->SetRotation(m_rotate);

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void TrapDoor::OnUpdate()
	{

	}
}