/*!
@file GoalDoor.cpp
@brief ゴールの扉
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	void GoalDoor::OnCreate()
	{
		//Drawコンポーネント
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);

		m_startPos = m_pos;

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void GoalDoor::OnUpdate()
	{

		if (!m_port) return;

		bool isConnect = m_port->GetConnect();
		if (isConnect)
		{
			m_isOpen = true;
			m_pos += m_moveDir * m_speed;
			m_trans->SetPosition(m_pos);

			float distance = (m_pos - m_startPos).length();

			if (distance >= m_scale.x || distance >= m_scale.y)
			{
				m_speed = 0;
			}
		}
		
	}
}