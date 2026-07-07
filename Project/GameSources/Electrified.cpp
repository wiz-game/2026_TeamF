/*!
@file Electrified.cpp
@brief 繧ｭ繝｣繝ｩ繧ｯ繧ｿ繝ｼ縺ｪ縺ｩ螳滉ｽ・
*/

#include "stdafx.h"
#include "Electrified.h"
#include "PowerSupply.h"

namespace basecross{

	//毎フレーム通電更新
	void Electrified::UpdateElectrified()
	{
		//電源元なら常にON、そうでなければ受け取った状態を反映
		m_currentPowered = m_isSource || m_nextPowered;

		//次フレームの状態をリセット
		m_nextPowered = false;

		//通電していないなら隣に伝えない
		if (!m_currentPowered)return;

		//接触している相手に電気を伝える
		for (auto& w : m_contactObjects)
		{
			if(auto obj = w.lock())
			{
				if (auto elec = obj->GetComponent<Electrified>(false))
				{
					elec->SetPowered();
				}
			}
		}
	}

	//接触開始
	void Electrified::OnElectrifiedEnter(std::shared_ptr<GameObject>& other)
	{
		if (other->GetComponent<Electrified>(false))
		{
			m_contactObjects.push_back(other);
		}
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
