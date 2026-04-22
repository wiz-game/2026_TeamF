/*!
@file Electrified.cpp
@brief ã‚­ãƒ£ãƒ©ã‚¯ã‚¿ãƒ¼ãªã©å®Ÿä½“
*/

#include "stdafx.h"
#include "Electrified.h"
#include "PowerSupply.h"
#include "GameObject.h"

namespace basecross{

	//ŠO•”‚©‚ç“d—Í‚ğ–á‚¤
	void Electrified::InjectEnergy(float amount)
	{
		m_nextEnergy += amount;
	}

	//–ˆƒtƒŒ[ƒ€’Ê“dXV
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

	//ÚGŠJn
	void Electrified::OnElectrifiedEnter(std::shared_ptr<GameObject>& other)
	{
		m_contactObjects.push_back(other);
	}

	//ÚGI—¹
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

		// PowerSupply ã‚’æŒã£ã¦ã„ã‚‹ã‹ç¢ºèª
		//auto power = target->GetComponent<PowerSupply>();
		//if (!power) return;

		// ãƒ•ãƒ¬ãƒ¼ãƒ æ™‚é–“ã‚’è€ƒæ…®ã—ã¦æµé‡ã‚’æ±ºå®š
		float delta = App::GetApp()->GetElapsedTime();

		// é›»æ°—ã‚’å—ã‘å–ã‚‹
		//float received = power->ConsumeElect(delta);

		//if (received > 0.0f)
		//{
		//}
	}
}
//end basecross
