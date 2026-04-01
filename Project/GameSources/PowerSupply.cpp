/*!
@file PowerSupply.cpp
@brief 電源実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"

namespace basecross {
	// 初期設定
	void PowerSupply::OnCreate()
	{
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 0.0f, -3.0f));
		m_transform->SetScale(Vec3(1.0f, 0.1f, 1.0f));

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		auto draw = AddComponent<PNTStaticDraw>();
		draw->SetMeshResource(L"DEFAULT_CUBE");
		draw->SetDiffuse(Col4(0, 1, 0, 1));

	}

	// 更新処理
	void PowerSupply::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
	}

	//インクとつながった時の処理
	void PowerSupply::Connected()
	{

	}
}
//end basecross

