/*!
@file MoveObj.cpp
@brief インク消去機体
*/


#include "stdafx.h"
#include "Project.h"

namespace basecross {

    void MoveObj::OnCreate()
    {
        BaseEnemy::OnCreate();

        m_transform->SetPosition(0, 0, 0);
        m_draw->SetDiffuse(Col4(0, 0, 1, 1));

        m_origin = m_transform->GetPosition();
        m_targetPos = m_origin;
    }

    // ============================================================
    // メイン制御（超シンプル）
    // ============================================================
    void MoveObj::OnUpdate()
    {
        UpdateGroundRay();

        auto stage = GetStage();

        std::vector<std::shared_ptr<GameObject>> inkObjs;
        stage->GetUsedTagObjectVec(L"InkCloud", inkObjs);

        auto target = FindNearestInk(inkObjs);


        if (!m_targetInk)
        {
            m_targetInk = FindNearestInk(inkObjs);
        }


        if (m_targetInk)
        {
            UpdateErase(m_targetInk);
        }
        else
        {
            UpdatePatrol();
        }

        wchar_t buf[256];
        swprintf(buf, 256, L"target:(%.2f, %.2f, %.2f)\n",
            m_targetPos.x, m_targetPos.y, m_targetPos.z);
        OutputDebugStringW(buf);

    }


    // ============================================================
    // ターゲット取得
    // ============================================================
    std::shared_ptr<InkCloud> MoveObj::FindNearestInk(
        const std::vector<std::shared_ptr<GameObject>>& objs)
    {
        Vec3 pos = m_transform->GetPosition();

        float minDist = FLT_MAX;
        std::shared_ptr<InkCloud> result = nullptr;

        for (auto& obj : objs)
        {
            auto ink = std::dynamic_pointer_cast<InkCloud>(obj);
            if (!ink) continue;

            auto trans = ink->GetComponent<Transform>();
            if (!trans) continue;

            float dist = (trans->GetPosition() - pos).length();

            if (dist < minDist)
            {
                minDist = dist;
                result = ink;
            }
        }

        return result;
    }


    // ============================================================
    // インク消去
    // ============================================================
    void MoveObj::UpdateErase(std::shared_ptr<InkCloud> target)
    {
        Vec3 pos = m_transform->GetPosition();

        auto trans = target->GetComponent<Transform>();
        if (!trans) return;

        Vec3 targetPos = trans->GetPosition();

        Vec3 toTarget = targetPos - pos;
        toTarget.y = 0.0f;

        float distance = toTarget.length();

        // 到達

        if (distance < 0.5f)
        {
            target->DestroyAllInk();
            target->DestroyGameObject();

            m_targetInk = nullptr; // ← 超重要

            return;
        }


        if (distance > 0.001f)
            toTarget.normalize();

        float delta = App::GetApp()->GetElapsedTime();
        pos += toTarget * m_moveSpeed * delta;

        if (m_isGround)
        {
            pos.y = m_groundY + m_heightOffset;
        }

        m_transform->SetPosition(pos);
    }


    // ============================================================
    // 徘徊
    // ============================================================
    void MoveObj::UpdatePatrol()
    {
        auto delta = App::GetApp()->GetElapsedTime();
        Vec3 pos = m_transform->GetPosition();

        Vec3 toTarget = m_targetPos - pos;
        toTarget.y = 0.0f;

        float distance = toTarget.length();

        if (distance < 0.5f)
        {
            float randX, randZ;

            do {
                randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * m_range;
                randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * m_range;

                m_targetPos = Vec3(
                    m_origin.x + randX,
                    m_origin.y,
                    m_origin.z + randZ
                );

            } while ((m_targetPos - pos).length() < 1.0f);

            return;
        }

        if (distance > 0.001f)
            toTarget.normalize();

        pos += toTarget * m_moveSpeed * delta;

        if (m_isGround)
        {
            pos.y = m_groundY + m_heightOffset;
        }

        m_transform->SetPosition(pos);
    }

}

//end basecross