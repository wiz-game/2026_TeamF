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
		BaseEnemy::UpdatePatrol();
	}
}
//end basecross