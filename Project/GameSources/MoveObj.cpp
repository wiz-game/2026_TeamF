/*!
@file MoveObj.cpp
@brief ベルトコンベア障害物
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	const float MoveObj::PUSH_POWER = 0.003f;
	const float MoveObj::GRAVITY = -9.8f;

	void MoveObj::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_transform = GetComponent<Transform>();

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
		float delta =
			App::GetApp()->GetElapsedTime();

		UpdateGravity(delta);
	}

	void MoveObj::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (dynamic_pointer_cast<BeltConveyor>(obj))
		{
			m_isGround = true;
			m_velocity = 0.0f;
		}
		if (dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = true;
			m_velocity = 0.0f;
		}
		if (auto player =
			std::dynamic_pointer_cast<Player>(obj))
		{
			PushPlayer(player);
		}
	}

	void MoveObj::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		if (auto player =
			std::dynamic_pointer_cast<Player>(obj))
		{
			PushPlayer(player);
		}
	}

	void MoveObj::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (dynamic_pointer_cast<BeltConveyor>(obj))
		{
			m_isGround = false;
		}
		if (dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = false;
		}
	}

	void MoveObj::UpdateMoveFloor(const Vec3& movePos)
	{
		m_currentMoveVec = movePos;
		Vec3 pos = m_transform->GetPosition();

		pos += movePos;

		m_transform->SetPosition(pos);
	}

	void MoveObj::PushPlayer(
		std::shared_ptr<Player> player)
	{
		Vec3 dir = m_currentMoveVec;

		dir.y = 0.0f;

		if (dir.length() > 0.001f)
		{
			dir.normalize();
		}

		player->AddVelocity(
			dir * PUSH_POWER
		);
	}
	void MoveObj::UpdateGravity(float delta)
	{
		Vec3 pos = m_transform->GetPosition();

		if (!m_isGround)
		{
			m_velocity += GRAVITY * delta;
			pos.y += m_velocity * delta;
		}
		else
		{
			m_velocity = 0.0f;
		}

		m_transform->SetPosition(pos);
	}
}