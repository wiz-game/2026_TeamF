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
		stage->GetUsedTagObjectVec(L"InkDraw", inkObjs);

		Vec3 pos = m_transform->GetPosition();

		for (auto& obj : inkObjs)
		{
			auto ink = std::dynamic_pointer_cast<InkDraw>(obj);
			if (!ink) continue;

			Vec3 inkPos = ink->GetComponent<Transform>()->GetPosition();

			float distance = (pos - inkPos).length();

			if (distance < 2.0f)
			{
				ink->DestroyGameObject();
			}
		}
	}
}
//end basecross