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
		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetParent(m_owner);
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);

		
		//Drawコンポーネント
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void TrapDoor::OnUpdate()
	{

	}

	//初期化
	void OffsetObject::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(m_scale);
		m_transform->SetParent(m_owner);
		m_transform->SetPosition(0, 0.1f, 0);
	}

	void OffsetObject::OnUpdate()
	{

	}

}