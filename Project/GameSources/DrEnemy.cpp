/*!
@file DrEnemy.cpp
@brief 敵対行動　インク描画機体
Project.hに対応する全てのヘッダをまとめている
*/

#include "stdafx.h"
#include "Project.h"

#include "Port.h"
#include "PowerSupply.h"

namespace basecross {

    //初期化
    void DrEnemy::OnCreate()
    {
        BaseEnemy::OnCreate();
        BaseEnemy::m_transform->SetPosition(0, 1, 0);
        m_origin = m_transform->GetPosition();
        BaseEnemy::m_draw->SetDiffuse(Col4(0.0f, 1.0f, 0.0f, 1.0f));

        SetUpBetWnnePoints();

        m_pointA.y = m_transform->GetPosition().y;
        m_pointB.y = m_transform->GetPosition().y;

		m_transform->SetPosition(m_pointA);
        m_state = State::Draw;
        m_moveSpeed = 2.0f;
    }

    void DrEnemy::OnUpdate()
    {
		BaseEnemy::OnUpdate();
    }

    void DrEnemy::SetUpBetWnnePoints()
    {
        auto objs = GetStage()->GetGameObjectVec();

        vector<shared_ptr<Port>> ports;
        vector<shared_ptr<PowerSupply>> supplies;


        for (auto& obj : objs)
        {
            if (auto port = dynamic_pointer_cast<Port>(obj))
            {
                ports.push_back(port);
            }

            if (auto supply = dynamic_pointer_cast<PowerSupply>(obj))
            {
                supplies.push_back(supply);
            }
        }

        Vec3 myPos = m_transform->GetPosition();
        shared_ptr<Port> nearesPort = nullptr;
        float minDist = FLT_MAX;

        for (auto& port : ports)
        {
            float dist = (port->GetComponent<Transform>()->GetPosition() - myPos).length();

            if (dist < minDist)
            {
                minDist = dist;
                nearesPort = port;
            }
        }

        shared_ptr<PowerSupply> nearesSupply = nullptr;
        minDist = FLT_MAX;

        Vec3 portPos = nearesPort->GetComponent<Transform>()->GetPosition();

        for (auto& supply : supplies)
        {
            float dist = (supply->GetComponent<Transform>()->GetPosition() - portPos).lengthSqr();

            if (dist < minDist)
            {
                minDist = dist;
                nearesSupply = supply;
            }

        }

        if (nearesSupply)
        {
            m_pointA = portPos;
            m_pointB = nearesSupply->GetComponent<Transform>()->GetPosition();

        }

    }

    void DrEnemy::UpdateInkDrow()
    {
        auto delta = App::GetApp()->GetElapsedTime();

        Vec3 pos = m_transform->GetPosition();
        Vec3 target = m_toB ? m_pointB : m_pointA;

        Vec3 targetXZ = target;
        targetXZ.y = pos.y;

        Vec3 dir = targetXZ - pos;
		dir.y = 0.0f; // Yは無視
        float dist = dir.length();

        if (dist > 0.001f)
        {
            dir.normalize();
        }

        // ★ 移動（Yはまだ触らない）
        pos += dir * m_moveSpeed * delta;

        // ★ 最後にYを確定
        if (m_isGround)
        {
            pos.y = m_groundY + m_heightOffset;
        }

        m_transform->SetPosition(pos);

        // インク処理
        m_inkTimer += delta;
        if (m_inkTimer >= m_inkInterval)
        {
            m_inkTimer = 0.0f;

            Vec3 inkPos = pos;
            inkPos.y -= 0.3f;

            SpanInk(inkPos);
        }

        // 到達判定
        if (dist < 0.5f)
        {
            if (m_toB)
            {
                m_toB = false;
            }
            else
            {
                m_state = State::Patrol;
            }
        }
    }

    void DrEnemy::UpdatePatrol()
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
}
//end basecross