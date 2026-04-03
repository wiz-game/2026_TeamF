/*!
@file UpDownFloor.cpp
@brief ゴール実体
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
		//m_coll->SetFixed(true);
		
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

		//通電していれば床が動く
		if (isConnect)
		{
			pos.y += m_moveSpeed * delta;
			if (pos.y > m_pos.y + 2.0f || pos.y <= 0.0f)
			{
				m_moveSpeed *= -1.0f;
			}

			m_transform->SetPosition(pos);
		}
	}
}
//end basecross
