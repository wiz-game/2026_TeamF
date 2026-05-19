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
        BaseEnemy::m_transform->SetPosition(0, 0, 0);
        BaseEnemy::m_draw->SetDiffuse(Col4(0.0f, 1.0f, 0.0f, 1.0f));

        SetUpBetWnnePoints();

        m_state = State::BetWeen;
        m_moveSpeed = 2.0f;
    }

    void DrEnemy::OnUpdate()
    {
        UpdateBetWeen();
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

            m_pointA = portPos;
            m_pointB = nearesSupply->GetComponent<Transform>()->GetPosition();
        }
    }

    void DrEnemy::UpdateBetWeen()
    {

        auto delta = App::GetApp()->GetElapsedTime();

        Vec3 pos = m_transform->GetPosition();

        Vec3 target = m_toB ? m_pointB : m_pointA;

        Vec3 dir = target - pos;
        float dist = dir.length();

        if (dist > 0.001f) {
            dir.normalize();
        }

        pos += dir * m_moveSpeed * delta;
        m_transform->SetPosition(pos);

        // 到達判定
        if (dist < 0.5f)
        {
            m_toB = !m_toB;
        }
    }

}
//end basecross