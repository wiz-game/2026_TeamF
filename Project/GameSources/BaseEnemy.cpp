/*!
@file BaseEnemy.cpp
@brief キャラクターなど実体
Project.hに対応する全てのヘッダをまとめている
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void BaseEnemy::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(1, 1, 1);
		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		m_state = State::Idle;

		m_origin = m_transform->GetPosition();
		m_targetPos = m_origin;

		m_isGround = false;
		m_groundCount = 0;
	}

	void BaseEnemy::OnUpdate()
	{
		UpdateGroundRay();
		
		switch (m_state) {
		case State::Idle:
			UpdateIdle();
			break;
		case State::Patrol:
			UpdatePatrol();
			break;
		case State::Chase:
			UpdateChase();
			break;
		case State::Draw:
			UpdateInkDrow();
			break;
		case State::Erase:
			UpdateInkErase();
			break;
		}

	}

	void BaseEnemy::UpdateIdle()
	{
		float idle = 0.0;
	}

	void BaseEnemy::UpdatePatrol()
	{
		auto delta = App::GetApp()->GetElapsedTime();
		Vec3 pos = m_transform->GetPosition();

		if (m_isGround)
		{
			pos.y = m_groundY + m_heightOffset;
		}

		Vec3 toTarget = m_targetPos - pos;
		toTarget.y = 0.0f;

		float distance = toTarget.length();

		// ✅ 到達判定は広めに
		if (distance < 0.5f)
		{
			float randX, randZ;

			// ✅ 近すぎるターゲットを防ぐ
			do {
				randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * m_range;
				randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * m_range;

				m_targetPos = Vec3(
					m_origin.x + randX,
					m_origin.y,
					m_origin.z + randZ
				);

			} while ((m_targetPos - pos).length() < 1.0f);

			return; // ✅ このフレームでは動かない
		}

		// ✅ 正規化は安全チェック付き
		if (distance > 0.001f) {
			toTarget.normalize();
		}

		pos += toTarget * m_moveSpeed * delta;

		if (m_isGround)
		{
			pos.y = m_groundY + m_heightOffset;
		}

		m_transform->SetPosition(pos);
	}

	void BaseEnemy::UpdateChase()
	{

	}

	void BaseEnemy::UpdateInkDrow(){}

	void BaseEnemy::UpdateInkErase(){}

	void BaseEnemy::SpanInk(const Vec3& pos)
	{
		auto ink = GetStage()->AddGameObject<InkDraw>();
		ink->GetComponent<Transform>()->SetPosition(pos);
	}

	void BaseEnemy::OnCollisionEnter(shared_ptr<GameObject>& obj)
	{

		if (auto floor = dynamic_pointer_cast<Floor>(obj))
		{
			Vec3 myPos = m_transform->GetPosition();
			Vec3 floorPos = floor->GetComponent<Transform>()->GetPosition();

			float diff = myPos.y - floorPos.y;

			// ★ 自分の足元に近い床だけ採用
			if (diff >= -0.5f && diff < 3.0f)
			{

				m_groundCount++;
				m_isGround = true;
				m_groundY = floorPos.y;
			}

		}
	}

	void BaseEnemy::OnCollisionExit(shared_ptr<GameObject>& obj)
	{
		if (dynamic_pointer_cast<Floor>(obj))
		{
			m_groundCount--;
			if (m_groundCount <= 0)
			{
				m_groundCount = 0;
				m_isGround = false;
			}
			
		}
	}

	void BaseEnemy::UpdateGroundRay()
	{

		Vec3 pos = m_transform->GetPosition();

		float nearestY = -FLT_MAX;
		bool found = false;

		auto objs = GetStage()->GetGameObjectVec();

		for (auto& obj : objs)
		{
			if (auto floor = dynamic_pointer_cast<Floor>(obj))
			{

				auto floorTr = floor->GetComponent<Transform>();

				Vec3 floorPos = floorTr->GetPosition();
				Vec3 floorScale = floorTr->GetScale();

				float topY = floorPos.y + floorScale.y * 1.5f;
				// ✅ 自分より下にある床だけ対象
				if (topY <= pos.y)
				{
					float diff = pos.y - topY;

					// ✅ 距離制限（Rayの長さ）
					if (diff < 5.0f)
					{
						if (topY > nearestY)
						{
							nearestY = topY;
							found = true;
						}
					}
				}
			}
		}

		if (found)
		{
			m_isGround = true;
			m_groundY = nearestY;
		}
		else
		{
			m_isGround = false;
		}
	}
}
//end basecross
