/*!
@file Electrified.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

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
		auto power = target->GetComponent<PowerSupply>();
		if (!power) return;

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
		if (isPower) {
			for (auto& target : targets) {
				Electrify(target);
			}
		}
	}
}
//end basecross
