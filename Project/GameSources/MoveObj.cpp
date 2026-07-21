/*!
@file MoveObj.cpp
@brief ベルトコンベア障害物
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	const float MoveObj::PUSH_POWER = 0.03f;

	void MoveObj::OnCreate()
	{
		GameObject::OnCreate();

		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);
		m_transform->SetPosition(m_pos);

		m_draw->SetDiffuse(Col4(1, 0, 0, 1));

		auto coll = AddComponent<CollisionObb>();
		coll->SetDrawActive(true);

		m_startPos = m_pos;
	}

	void MoveObj::OnUpdate()
	{
		OutputDebugStringW(L"Hit\n");

		float delta =
			App::GetApp()->GetElapsedTime();

		m_moveAmount += m_moveSpeed * delta;

		Vec3 dir = m_moveDirection;

		if (dir.length() > 0.001f)
		{
			dir.normalize();
		}

		Vec3 pos = m_startPos + dir * m_moveAmount;

		m_transform->SetPosition(pos);
	}

	void MoveObj::OnCollisionEnter(
		std::shared_ptr<GameObject>& obj)
	{
		if (auto player =
			std::dynamic_pointer_cast<Player>(obj))
		{
			PushPlayer(player);
		}
	}

	void MoveObj::OnCollisionExcute(
		std::shared_ptr<GameObject>& obj)
	{
		if (auto player =
			std::dynamic_pointer_cast<Player>(obj))
		{
			PushPlayer(player);
		}
	}

	void MoveObj::PushPlayer(
		std::shared_ptr<Player> player)
	{
		Vec3 dir = m_moveDirection;

		dir.y = 0.0f;

		if (dir.length() > 0.001f)
		{
			dir.normalize();
		}

		player->AddVelocity(
			dir * PUSH_POWER
		);
	}

}