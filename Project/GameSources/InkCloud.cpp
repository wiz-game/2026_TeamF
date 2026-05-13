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

		auto electrified = AddComponent<Electrified>();
	}

	void InkCloud::OnUpdate()
	{
		auto delta = App::GetApp()->GetElapsedTime();
		if (isPower)
		{
			m_powerTimer += delta;

			// --- ループ処理の追加 ---
			float cycleTime = 1.0f; // 1.0秒ごとに電流が流れる周期
			if (m_powerTimer > cycleTime)
			{
				m_powerTimer = 0.0f; // タイマーをリセットして繰り返す
			}

			// 電流移動のエフェクト計算
			float effectDuration = 0.5f; // 1周期のうち、光っている時間

			for (auto& ink : m_inkList) {
				// 現在のタイマーが発光期間内であれば光らせる
				if (m_powerTimer < effectDuration) {
					// 時間の経過とともに少しずつ暗くする（フェードアウト）
					float alpha = 1.0f - (m_powerTimer / effectDuration);
					ink->SetInkColor(1, 1, 0, alpha);
				}
				else {
					// 発光期間を過ぎたら次の周期まで黒
					ink->SetInkColor(0, 0, 0, 1);
				}
			}
		}
		else
		{
			m_powerTimer = 0.0f;
			for (auto& ink : m_inkList)
			{
				ink->SetInkColor(0, 0, 0, 1);
			}
		}
		m_wasPower = isPower;
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
			bool power = powerSupply->GetisPower();
			isPower = power;
		}
		// port
		if (auto port = std::dynamic_pointer_cast<Port>(info))
		{
			port->SetisPower(isPower);
			//m_wasPower = true;
		}

		//自分自身都の判定
		if (auto otherCloud = std::dynamic_pointer_cast<InkCloud>(info))
		{
			// 相手が通電していて自分がしていなければ、電気をもらう
			if (otherCloud->GetIsPower() && !this->isPower)
			{
				this->isPower = true;
				this->m_powerTimer = otherCloud->m_powerTimer - 0.2f;
			}
		}
	}
	void InkCloud::OnCollisionExcute(shared_ptr<GameObject>& info)
	{
		OnCollisionEnter(info);
	}

	void InkCloud::OnCollisionExit(shared_ptr<GameObject>& info)
	{
	}

}
//end basecross
