/*!
@file ErEnemy.cpp
@brief インク消去機体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void ErEnemy::OnCreate()
	{
		BaseEnemy::OnCreate();
		BaseEnemy::m_transform->SetPosition(0, 0, 0);
		BaseEnemy::m_draw->SetDiffuse(Col4(0.0f, 0.0f, 1.0f, 1.0f));

		m_state = State::Erase;
	}

	void ErEnemy::OnUpdate()
	{
		BaseEnemy::OnUpdate();
	}

	void ErEnemy::UpdateInkErase()
	{
		auto stage = GetStage();

		std::vector<std::shared_ptr<GameObject>> inkObjs;
		stage->GetUsedTagObjectVec(L"InkCloud", inkObjs);

		Vec3 pos = m_transform->GetPosition();

		if (inkObjs.empty())
		{
			m_state = State::Patrol;
			return;
		}

		float minDist = FLT_MAX;
		std::shared_ptr<InkCloud> targetInk = nullptr;

		for (auto& obj : inkObjs)
		{
			auto ink = std::dynamic_pointer_cast<InkCloud>(obj);
			if (!ink) continue;

			Vec3 inkPos = ink->GetComponent<Transform>()->GetPosition();

			float diff = (inkPos - pos).length();

			if (diff < minDist)
			{
				minDist = diff;
				targetInk = ink;
			}
		}

		if (!targetInk)return;

		Vec3 inkPos = targetInk->GetComponent<Transform>()->GetPosition();
		Vec3 toTarget = inkPos - pos;
		toTarget.y = 0.0f;

		float distance = toTarget.length();

		if (distance < 1.0f)
		{
			targetInk->DestroyAllInk();
			targetInk->DestroyGameObject();
			return;
		}

		if (distance > 0.001f)
		{
			toTarget.normalize();
		}

		float delta = App::GetApp()->GetElapsedTime();
		pos += toTarget * m_moveSpeed * delta;

		m_transform->SetPosition(pos);
	}



	void ErEnemy::UpdatePatrol()
	{
		auto stage = GetStage();

		std::vector<std::shared_ptr<GameObject>> inkObjs;
		stage->GetUsedTagObjectVec(L"InkCloud", inkObjs);

		if (!inkObjs.empty())
		{
			m_state = State::Erase;
			return;
		}

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
}
//end basecross