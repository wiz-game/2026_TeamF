/*!
@file Port.cpp
@brief ポート実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "Port.h"

namespace basecross {
	// 初期設定
	void Port::OnCreate()
	{
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"Port", path + L"Port.png"); // 画像ファイルを読み込んでアセットとして登録する
			app->RegisterTexture(L"Black", path + L"Black.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);

		m_staticDraw = AddComponent<Texture2DrawComp>();
		m_staticDraw->CreateTexture(m_scale.x, m_scale.z);
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(0, 0, 0, 1));
		m_staticDraw->SetTextureResource(L"Port");
		m_staticDraw->SetTexture2(L"Black");
		m_staticDraw->SetBlendState(BlendState::AlphaBlend);
		
		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		InkConnectChecker::Get().AddPort(GetThis<Port>());

		AddComponent<TextureCollision>();
	}

	// 更新処理
	void Port::OnUpdate()
	{
		// 電流の状態に応じて接続状態と色を更新
		if (isConnect)
		{
			m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
		}
		else
		{
			m_staticDraw->SetDiffuse(Col4(0.5f, 0.5f, 0.5f, 1));
		}
	}
}
//end basecross

