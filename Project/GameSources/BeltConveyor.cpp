/*!
@file BeltConveyor.cpp
@brief ベルトコンベアー
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void BeltConveyor::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		//m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetEmissive(Col4(0, 0, 1, 1));

		//m_coll = AddComponent<CollisionObb>();
		//m_coll->SetFixed(true);

		m_floorDec = GetStage()->AddGameObject<FloorDecision>(GetThis<BeltConveyor>());

		try
		{	// objectの取得
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		}
		catch (...) {
		}
	}

	void BeltConveyor::OnUpdate()
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
			m_isMove = true;
			m_floorDec->SetCurrentMoveVec(Vec3(0, 0, m_speed * delta));
		}
	}
}
//end basecross
