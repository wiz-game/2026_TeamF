/*!
@file UpDownFloor.cpp
@brief 上下する床実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void UpDownFloor::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		//m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetEmissive(Col4(0, 0, 0, 1));

		m_coll = AddComponent<CollisionObb>();
		m_coll->SetFixed(true);

		try
		{	// objectの取得
			m_port = GetStage()->GetSharedGameObject<Port>(L"Port");
		}
		catch (...) {
			m_port.reset();
		}
	}

	void UpDownFloor::OnUpdate()
	{
		if (!m_port) return; // ポートがいなければ何もしない

		auto scene = App::GetApp()->GetScene<Scene>();
		bool isConnect = m_port->GetConnect();
		float delta = App::GetApp()->GetElapsedTime();
		Vec3 pos = m_transform->GetPosition();
		Vec3 newPos = pos; // 移動後の位置を計算するための変数

		//通電していれば床が動く
		if (isConnect)
		{
			m_isUp = true;
			newPos.y += m_moveSpeed * delta;
			if (newPos.y > m_pos.y + 3.0f || newPos.y <= -0.2f)
			{
				m_moveSpeed *= -1.0f;
			}

			// --- プレイヤーへの移動量加算処理 ---
			// このフレームの移動量を算出
			Vec3 moveAmount = newPos - pos;

			// プレイヤーを取得（共有オブジェクトから取得する例）
			auto player = GetStage()->GetSharedGameObject<Player>(L"Player");
			if (player) {
				Vec3 pPos = player->GetComponent<Transform>()->GetPosition();
				// プレイヤーが床の範囲内にいるか判定
				float diffX = fabsf(pPos.x - pos.x);
				float diffZ = fabsf(pPos.z - pos.z);

				// 床のサイズ(2.0x2.0)に基づき、範囲内(1.0)かつ足元が表面に近いかチェック
				if (diffX < 1.1f && diffZ < 1.1f &&
					pPos.y > pos.y && pPos.y < pos.y + 1.5f)
				{
					player->AddExternalMove(moveAmount);
					player->SetExternalVelocity(Vec3(0, m_moveSpeed, 0));
				}
			}
			m_transform->SetPosition(newPos);
		}
		else
		{
			m_isUp = false;
		}
	}
}
//end basecross
