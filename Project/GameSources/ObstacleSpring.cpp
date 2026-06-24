/*!
@file ObstacleSpring.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//初期化
	void ObstacleSpring::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CYLINDER");
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(1, 2, 1);
		m_transform->SetPosition(0, 1, 0);
		m_transform->SetRotation(0, 0, XM_PIDIV2);
		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CYLINDER");

		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Spring", path + L"Spring.png");
		m_draw->SetTextureResource(L"Spring");
	}

	void ObstacleSpring::OnUpdate()
	{

	}
}
//end basecross
