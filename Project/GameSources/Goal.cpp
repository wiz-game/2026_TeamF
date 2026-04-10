/*!
@file Goal.cpp
@brief ゴール実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Goal::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		//m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDrawActive(false);//最初は描画しない
		m_staticDraw->SetRasterizerState(RasterizerState::Wireframe);//ワイヤーフレームで描画
		m_staticDraw->SetBlendState(BlendState::Additive);//加算ブレンドで描画

		m_staticDraw->SetDiffuse(Col4(1.0f, 1.0f, 1.0f, 1.0f));//緑色

		try
		{	// objectの取得
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
			m_port = GetStage()->GetSharedGameObject<Port>(L"Port");
		}
		catch (...) {
			m_player.reset();
			m_port.reset();
		}
	}

	void Goal::OnUpdate()
	{
		if (!m_player) return; // プレイヤーがいなければ何もしない

		auto scene = App::GetApp()->GetScene<Scene>();
		bool isConnect = m_port->GetConnect();

		//通電していれば表示
		if (isConnect)
		{
			m_staticDraw->SetDrawActive(true);
			// 自分の位置とプレイヤーの位置を取得
			Vec3 myPos = m_transform->GetPosition();
			// プレイヤーのTransformコンポーネントから位置を取得
			Vec3 playerPos = m_player->GetComponent<Transform>()->GetPosition();

			// 位置の差を計算
			float diffX = fabs(myPos.x - playerPos.x);
			float diffY = fabs(myPos.y - playerPos.y);
			float diffZ = fabs(myPos.z - playerPos.z);

			// 距離が一定以下（例: 1.5f）ならゴールとみなす
			if (diffX < 1.8f && diffY < 1.8f && diffZ < 0.45f)
			{
				scene->SetDebugString(L"Goal");
			}
		}
	}
}
//end basecross
