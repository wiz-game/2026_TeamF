/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{
	// プレイヤーの初期設定
	void Player::OnCreate()
	{
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 0.0f, 0.0f));
		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetDiffuse(Col4(1, 1, 1, 1));
	}

	// プレイヤーの更新処理
	void Player::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		// 前回からの経過時間：デルタタイムを取得する
		float delta = app->GetElapsedTime();
	}
}
//end basecross

