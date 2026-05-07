/*!
@file BaseEnemy.cpp
@brief キャラクターなど実体
Project.hに対応する全てのヘッダをまとめている
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void BaseEnemy::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(1, 1, 1);
	}

}
//end basecross
