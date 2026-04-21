/*!
@file Electrified.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Electrified.h"
#include "PowerSupply.h"
#include "GameObject.h"

namespace basecross{

	void Electrified::InjectEnergy(float amount)
	{
		m_nextEnergy += amount;
	}

	void Electrified::UpdateElectrifed()
	{
		m_energy = m_nextEnergy;
		m_nextEnergy = 0.0f;

		if (m_energy <= 0.0f)return;

		for (auto& w : m_contactObjects)
		{
			if(auto obj = w.lock())
			{
				if (auto elec = obj->GetComponent<Electrified>())
				{
					elec->InjectEnergy(m_energy);
				}
			}
		}
	}

	void Electrified::OnElectrifiedEnter(std::shared_ptr<GameObject>& other)
	{
		m_contactObjects.push_back(other);
	}

	void Electrified::OnElectrifiedExit(std::shared_ptr<GameObject>& other)
	{
		m_contactObjects.erase(
			std::remove_if(
				m_contactObjects.begin(), 
				m_contactObjects.end(),
				[&](const std::weak_ptr<GameObject>& w) {
					return w.lock() == other;
				}),
			m_contactObjects.end()
		);

		// PowerSupply を持っているか確認
		//auto power = target->GetComponent<PowerSupply>();
		//if (!power) return;

		// フレーム時間を考慮して流量を決定
		float delta = App::GetApp()->GetElapsedTime();

		// 電気を受け取る
		//float received = power->ConsumeElect(delta);

		//if (received > 0.0f)
		//{
		//}
	}
}
//end basecross
