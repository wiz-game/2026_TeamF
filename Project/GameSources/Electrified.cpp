/*!
@file Electrified.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Electrified.h"
#include "PowerSupply.h"

namespace basecross{

	//初期化
	void Electrified::OnCreate()
	{
	}

	void Electrified::OnCollisionEnter(shared_ptr<GameObject>& info)
	{
		targets.push_back(info);
	}

	void Electrified::OnCollisionExit(shared_ptr<GameObject>& info)
	{
		targets.erase(std::remove(targets.begin(), targets.end(), info), targets.end());
	}

	void Electrified::Electrify(shared_ptr<GameObject>& target)
	{

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

	void Electrified::OnUpdate()
	{
		if (!m_isSource) isPower = false;// 電源でない場合は電気を受け取る前にリセット

		if (isPower) {
			for (auto& target : targets) {
				//Electrify(target);
				//相手がElectrified（またはその継承クラス）か確認
				auto targetElect = std::dynamic_pointer_cast<Electrified>(target);
				if (targetElect) {
					targetElect->isPower = true;// 電気を伝える
				}
			}
		}
	}
}
//end basecross
