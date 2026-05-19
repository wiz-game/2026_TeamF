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

        float minDist = FLT_MAX;

        for (auto& port : ports)
        {
            for (auto& supply : supplies)
            {
                Vec3 pPos = port->GetComponent<Transform>()->GetPosition();
                Vec3 sPos = supply->GetComponent<Transform>()->GetPosition();

                float dist = (pPos - sPos).length();

                if (dist < minDist)
                {
                    minDist = dist;
                    m_pointA = pPos;
                    m_pointB = sPos;
                }
            }
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