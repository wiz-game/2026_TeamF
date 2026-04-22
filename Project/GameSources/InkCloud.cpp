/*!
@file InkCloud.cpp
@brief インクのコリジョンをまとめるクラスの実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	//初期化
	void InkCloud::OnCreate()
	{
		AddTag(L"InkCloud");
		//GetStage()->SetSharedGameObject(L"InkCloud", GetThis<InkCloud>());
	}

	void InkCloud::OnUpdate()
	{
		// 電気が通っていたら、中に入っている全インクを光らせる
		for (auto& ink : m_inkList) {
			if (isPower) {
				ink->SetInkColor(1, 1, 0, 1); // 黄色く光る
			}
			else {
				ink->SetInkColor(0, 0, 0, 1); // 黒に戻る
			}
		}
	}

	//InkDrawを1つのコリジョンにまとめる処理
	void InkCloud::UpdateCombinaedCollision()
	{
		if (m_inkList.empty()) return;

		Vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);
		for (auto& ink : m_inkList)
		{
			Vec3 pos = ink->GetComponent<Transform>()->GetPosition();
			Vec3 scale = ink->GetComponent<Transform>()->GetScale() * 0.5f; // コリジョンの半分のサイズ
			Vec3 inkMin = pos - scale;
			Vec3 inkMax = pos + scale;
			if (m_combinedColl) {
				minPos.x = min(minPos.x, inkMin.x);
				minPos.y = min(minPos.y, inkMin.y);
				minPos.z = min(minPos.z, inkMin.z);

				maxPos.x = max(maxPos.x, inkMax.x);
				maxPos.y = max(maxPos.y, inkMax.y);
				maxPos.z = max(maxPos.z, inkMax.z);
			}
			else {
				minPos = inkMin;
				maxPos = inkMax;
			}
		}

		//中心点とサイズを計算
		Vec3 center = (minPos + maxPos) * 0.5f;
		Vec3 size = maxPos - minPos;
		if (size.y < 0.1f) size.y = 0.2f;//厚みがないと判定が消えるので補正

		if (!m_combinedColl)
		{
			m_combinedColl = AddComponent<CollisionObb>();
			m_combinedColl->SetAfterCollision(AfterCollision::None);
		}

		auto transform = GetComponent<Transform>();
		transform->SetPosition(center);
		transform->SetScale(size);
		m_combinedColl->SetDrawActive(false);
	}

	void InkCloud::OnCollisionEnter(std::shared_ptr<GameObject>& info)
	{
		if (auto powerSupply = std::dynamic_pointer_cast<PowerSupply>(info))
		{
			float elect = powerSupply->GetElect();
			bool power = powerSupply->GetisPower();
			isPower = power;
			m_elect = power ? elect : 0.0f;

		}
		// port
		if (auto port = std::dynamic_pointer_cast<Port>(info))
		{
			port->SetisPower(isPower, isPower ? m_elect : 0.0f);
		}

		//自分自身都の判定
		if (auto otherCloud = std::dynamic_pointer_cast<InkCloud>(info))
		{
			// 相手が通電していて自分がしていなければ、電気をもらう
			if (otherCloud->GetisPower() && !this->isPower)
			{
				this->isPower = true;
				// m_elect も必要なら渡す
			}
		}
	}
		void InkCloud::OnCollisionExit(shared_ptr<GameObject>& info)
	{
	}

}
//end basecross
