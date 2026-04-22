/*!
@file Electrified.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Electrified.h"
#include "PowerSupply.h"
#include "GameObject.h"

namespace basecross{

	//外部から電力を貰う
	void Electrified::InjectEnergy(float amount)
	{
		m_nextEnergy += amount;
	}

	//毎フレーム通電更新
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

	//接触開始
	void Electrified::OnElectrifiedEnter(std::shared_ptr<GameObject>& other)
	{
		m_contactObjects.push_back(other);
	}

	//接触終了
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
	}
}
//end basecross
