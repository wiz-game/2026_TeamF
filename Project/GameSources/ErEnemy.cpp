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

		m_state = State::Patrol;
	}

	void ErEnemy::OnUpdate()
	{

		UpdateGroundRay();

		auto stage = GetStage();

		std::vector<std::shared_ptr<GameObject>> inkObjs;
		stage->GetUsedTagObjectVec(L"InkCloud", inkObjs);

		std::shared_ptr<InkCloud> target = FindNearestInk(inkObjs);

		if (target)
		{
			UpdateErase(target);
		}
		else
		{
			UpdatePatrol();
		}

	}

    void ErEnemy::UpdateInkErase(std::shared_ptr<InkCloud> target)
    {
		Vec3 pos = m_transform->GetPosition();
		Vec3 targetPos = target->GetComponent<Transform>()->GetPosition();

		Vec3 toTarget = targetPos - pos;
		toTarget.y = 0.0f;

		float distance = toTarget.length();

		if (distance < 0.5f)
		{
			target->DestroyAllInk();
			target->DestroyGameObject();
			return;
		}

		if (distance > 0.001f)
			toTarget.normalize();

		float delta = App::GetApp()->GetElapsedTime();
		pos += toTarget * m_moveSpeed * delta;

		m_transform->SetPosition(pos);

    }

	void ErEnemy::UpdatePatrol()
	{

		if (m_justErased)
		{
			m_justErased = false;
			return;
		}

		auto stage = GetStage();

		std::vector<std::shared_ptr<GameObject>> inkObjs;
		stage->GetUsedTagObjectVec(L"InkCloud", inkObjs);

        Vec3 pos = m_transform->GetPosition();

        std::shared_ptr<InkCloud> candidate = nullptr;
        float minDist = FLT_MAX;

        for (auto& obj : inkObjs)
        {
            auto ink = std::dynamic_pointer_cast<InkCloud>(obj);
            if (!ink) continue;

            auto trans = ink->GetComponent<Transform>();
            if (!trans) continue;

            float dist = (trans->GetPosition() - pos).length();
            if (dist < minDist)
            {
                minDist = dist;
                candidate = ink;
            }
        }

        // ★ここが核心
        if (candidate)
        {
            m_targetInk = candidate;     // 先にターゲット確保
            m_state = State::Erase;      // その後遷移
            return;
        }


		auto delta = App::GetApp()->GetElapsedTime();

		Vec3 toTarget = m_targetPos - pos;
		toTarget.y = 0.0f;

		float distance = toTarget.length();

		// 到達判定は広めに
		if (distance < 0.5f)
		{
			float randX, randZ;

			// 近すぎるターゲットを防ぐ
			do {
				randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * m_range;
				randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * m_range;

				m_targetPos = Vec3(
					m_origin.x + randX,
					m_origin.y,
					m_origin.z + randZ
				);

			} while ((m_targetPos - pos).length() < 1.0f);

			return; // このフレームでは動かない
		}

		// 正規化は安全チェック付き
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