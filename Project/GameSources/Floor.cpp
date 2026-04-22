/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	void Floor::OnCreate()
	{
		//Drawコンポーネント
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void Floor::OnUpdate()
	{

	}
}