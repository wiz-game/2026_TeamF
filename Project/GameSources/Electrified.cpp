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
		float strength = e; //電気の強さを取得
		//電気の影響を受けるオブジェクトに対して、電気の強さを加える処理をここに書く
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
