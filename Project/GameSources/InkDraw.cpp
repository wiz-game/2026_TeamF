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
		m_draw->SetDiffuse(Col4(m_color));
		m_draw->SetEmissive(Col4(m_color));

		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);
		coll->SetMakedSize(0.0001f);
	}

	void InkDraw::OnUpdate()
	{
		
		Electrified::OnUpdate();
	}

	void InkDraw::FadingInk(float amount)
	{
		m_color.x = max(0.0f, m_color.x + amount);
		m_color.y = max(0.0f, m_color.y + amount);
		m_color.z = max(0.0f, m_color.z + amount);
		m_draw->SetDiffuse(m_color);
		m_draw->SetEmissive(m_color);
	}
}
//end basecross
