/*!
@file PowerSupply.cpp
@brief 電源実体
*/

#include "stdafx.h"
#include "PowerSupply.h"
#include "Project.h"
#include "game_controller.h"
#include "InkDraw.h"
#include "TextureCollision.h"
namespace basecross {
	// 初期設定
	void PowerSupply::OnCreate()
	{
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"PowerSupply", path + L"PowerSupply.png"); // 画像ファイルを読み込んでアセットとして登録する
			app->RegisterTexture(L"Black", path + L"Black.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}

		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);

		// ドローコンポーネントを追加
		m_staticDraw = AddComponent<Texture2DrawComp>();
		m_staticDraw->CreateTexture(m_scale.x, m_scale.z);
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
		m_staticDraw->SetTextureResource(L"PowerSupply");
		m_staticDraw->SetTexture2(L"Black");
		m_staticDraw->SetOwnShadowActive(true);

		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		InkConnectChecker::Get().AddPowerSupply(GetThis<PowerSupply>());
		AddComponent<TextureCollision>();
	}

	// 更新処理
	void PowerSupply::OnUpdate()
	{
	}

}
//end basecross

