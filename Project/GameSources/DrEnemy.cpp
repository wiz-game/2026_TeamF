/*!
@file DrEnemy.cpp
@brief 敵対行動　インク描画機体
Project.hに対応する全てのヘッダをまとめている
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void DrEnemy::OnCreate()
	{
		BaseEnemy::OnCreate();
		BaseEnemy::m_transform->SetPosition(5, 1, 0);
		BaseEnemy::m_draw->SetDiffuse(Col4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	void DrEnemy::OnUpdate()
	{
		BaseEnemy::UpdateInkDrow();
	}

}
//end basecross