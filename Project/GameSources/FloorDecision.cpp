/*!
@file UpDownFloor.cpp
@brief 上下する床実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void FloorDecision::OnCreate()
	{
		GetStage()->SetSharedGameObject(L"FloorDecision", GetThis<FloorDecision>());
		try
		{	// objectの取得
			m_updownFloor = GetStage()->GetSharedGameObject<UpDownFloor>(L"UpDownFloor");
		}
		catch (...) {
			m_updownFloor.reset();
		}
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(m_scale);
		m_transform->SetParent(m_updownFloor);

		m_coll = AddComponent<CollisionObb>();
		m_coll->SetAfterCollision(AfterCollision::None);
		m_coll->SetDrawActive(false);
		m_coll->SetFixed(true);

	}

	void FloorDecision::OnUpdate()
	{
		if (!m_updownFloor) return; // ポートがいなければ何もしない

		auto scene = App::GetApp()->GetScene<Scene>();
	}

	void FloorDecision::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = dynamic_pointer_cast<Player>(obj))
		{
			auto pTrans = player->GetComponent<Transform>();
			auto pPos = m_transform->GetWorldPosition();
			auto pScale = m_transform->GetScale(); // 判定エリアの大きさ
			
			// 判定エリアのX軸とZ軸の「端」を計算
			float xMin = pPos.x - (pScale.x / 2.0f);
			float xMax = pPos.x + (pScale.x / 2.0f);
			float zMin = pPos.z - (pScale.z / 2.0f);
			float zMax = pPos.z + (pScale.z / 2.0f);

			//プレイヤーの中心(pPos)が、エリアのXとZの範囲内に完全に入っているか判定
			if (pPos.x >= xMin && pPos.x <= xMax &&
				pPos.z >= zMin && pPos.z <= zMax)
			{
				m_getOn = true;
			}
			else
			{
				// 範囲外なら下ろす
				m_getOn = false;
			}
		}

	}
	void FloorDecision::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		OnCollisionEnter(obj);//Enterと同じ処理を行う
		//if (auto player = dynamic_pointer_cast<Player>(obj))
		//{
		//	auto pTrans = player->GetComponent<Transform>();
		//	auto pPos = pTrans->GetPosition();
		//	auto decisionTop = pTrans->GetPosition() + (pTrans->GetScale() / 2.0f);
		//	if (pPos.x <= decisionTop.x || pPos.z <= decisionTop.z)
		//	{
		//		m_getOn = true;
		//	}
		//}

	}
	void FloorDecision::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = dynamic_pointer_cast<Player>(obj))
		{
			//auto pTrans = player->GetComponent<Transform>();
			//auto pPos = pTrans->GetPosition();
			//auto decisionTop = pTrans->GetPosition() + (pTrans->GetScale() / 2.0f);
			//if (pPos.x >= decisionTop.x || pPos.z >= decisionTop.z)
			//{
				m_getOn = false;
			//}
		}
	}
}
//end basecross
