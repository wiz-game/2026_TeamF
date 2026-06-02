/*!
@file ErEnemy.cpp
@brief インク消去機体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void ErEnemy::OnCreate()
	{
		BaseEnemy::OnCreate();
		BaseEnemy::m_transform->SetPosition(0, 0, 0);
		BaseEnemy::m_draw->SetDiffuse(Col4(0.0f, 0.0f, 1.0f, 1.0f));
	}

	void ErEnemy::OnUpdate()
	{
		UpdateInkErase();
	}

	void ErEnemy::UpdateInkErase()
	{
		auto stage = GetStage();

		std::vector<std::shared_ptr<GameObject>> inkObjs;
		stage->GetUsedTagObjectVec(L"InkCloud", inkObjs);

		Vec3 pos = m_transform->GetPosition();

		for (auto& obj : inkObjs)
		{
			auto ink = std::dynamic_pointer_cast<InkCloud>(obj);
			if (!ink) continue;

			Vec3 inkPos = ink->GetComponent<Transform>()->GetPosition();

			Vec3 diff = pos - inkPos;

			if (diff.lengthSqr() < 5.0f)
			{
				ink->SetDrawActive(false);
			}
		}
	}
}
//end basecross