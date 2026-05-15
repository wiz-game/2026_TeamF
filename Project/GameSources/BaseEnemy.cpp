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

		m_state = State::Idle;

		m_origin = m_transform->GetPosition();
		m_targetPos = m_origin;
	}

	void BaseEnemy::OnUpdate()
	{
		switch (m_state) {
		case State::Idle:
			UpdateIdle();
			break;
		case State::Patrol:
			UpdatePatrol();
			break;
		case State::BetWeen:
			UpdateBetWeen();
			break;
		case State::Drow:
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
		m_moveSpeed = idle;
	}

	void BaseEnemy::UpdatePatrol()
	{
		auto delta = App::GetApp()->GetElapsedTime();
		Vec3 pos = m_transform->GetPosition();

		float fixedY = 1.0f;
		pos.y = fixedY;

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
					fixedY,
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
		pos.y = fixedY;

		m_transform->SetPosition(pos);
	}

	void BaseEnemy::UpdateBetWeen()
	{

	}

	void BaseEnemy::UpdateInkDrow()
	{
		Vec3 pos = m_transform->GetPosition();
		auto delta = App::GetApp()->GetElapsedTime();
		pos.x += 1.0 * delta;
		m_transform->SetPosition(pos);
	}

	void BaseEnemy::UpdateInkErase()
	{
		Vec3 pos = m_transform->GetPosition();
		auto delta = App::GetApp()->GetElapsedTime();
		pos.y += 1.0 * delta;
		m_transform->SetPosition(pos);

	}
}
//end basecross
