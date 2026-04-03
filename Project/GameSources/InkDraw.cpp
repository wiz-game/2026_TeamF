/*!
@file InkDraw.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//初期化
	void InkDraw::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(Vec3(5.0f, 0.0f, 0.0f));
		m_transform->SetRotation(Vec3(XM_PIDIV2, 0.0f, 0.0f));
		m_transform->SetScale(Vec3(m_scaleX, m_scaleY, 0.5f));
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SQUARE");
		m_draw->SetDiffuse(Col4(0, 0, 0, 1));
		m_draw->SetEmissive(Col4(0, 0, 0, 1));

		//auto coll = AddComponent<CollisionObb>();
	}

	void InkDraw::OnUpdate()
	{
	}
}
//end basecross
