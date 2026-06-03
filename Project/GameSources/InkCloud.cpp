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
		AddComponent<Electrified>();
	}

	void InkCloud::OnUpdate()
	{
		auto delta = App::GetApp()->GetElapsedTime();
		auto elec = GetComponent<Electrified>();
		//電流の更新
		elec->UpdateElectrified();
		isPower = elec->IsPowered();//電流が流れているかどうかを更新

		if (isPower)
		{
			m_powerTimer += delta;

			// --- ループ処理の追加 ---
			float cycleTime = 1.0f; // 1.0秒ごとに電流が流れる周期
			if (m_powerTimer > cycleTime)
			{
				m_powerTimer = 0.0f; // タイマーをリセットして繰り返す
				if (m_ElectricSound) {
					SoundManager::Get().Stop(m_ElectricSound);
				}
				m_ElectricSound = SoundManager::Get().PlaySE(L"ELECTRIC", 0.1f);
				
			}
			if (m_ElectricEffectHandle == -1) {
				Vec3 effectPosition = GetComponent<Transform>()->GetPosition() + Vec3();
				EffectManager::g_Instance->PlayEffect(m_ElectricEffectHandle, L"ELECTRIC", effectPosition, 0.0f);
			}
			// 電流移動のエフェクト計算
			float effectDuration = 0.5f; // 1周期のうち、光っている時間

			for (auto& ink : m_inkList) {
				for (size_t i = 0; i < m_inkList.size(); i++) 
				{
					// 各インクの位置に応じて、タイマーをずらす（例: インクの位置に基づいて遅延を計算）
					float delay = (i * 0.1f); // インクごとに0.1秒の遅延
					float localTimer = fmod(m_powerTimer + delay, cycleTime);

					// タイマーが負の値になる可能性があるため、正の値に変換
					if(localTimer < 0)	localTimer += cycleTime;

					// 現在のタイマーが発光期間内であれば光らせる
					if (localTimer < effectDuration) {
						// 時間の経過とともに少しずつ暗くする（フェードアウト）
						float alpha = 1.0f - (localTimer / effectDuration);
						ink->SetInkColor(1, 1, 0, alpha);
					}
					else {
						// 発光期間を過ぎたら次の周期まで黒
						ink->SetInkColor(0, 0, 0, 1);
					}
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
	void InkCloud::OnDestroy() {
		if (m_ElectricSound) {
			SoundManager::Get().Stop(m_ElectricSound);
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
		if (auto elec = GetComponent<Electrified>(false))
		{
			//リストに追加
			elec->OnElectrifiedEnter(info);
		}
	}
	void InkCloud::OnCollisionExcute(shared_ptr<GameObject>& info)
	{
	}

	void InkCloud::OnCollisionExit(shared_ptr<GameObject>& info)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			elec->OnElectrifiedExit(info);
		}
	}

}
//end basecross
