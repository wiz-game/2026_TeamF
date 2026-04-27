/*!
@file MoveFloor.cpp
@brief 上下する床実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void MoveFloor::OnCreate()
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

		m_floorDec = GetStage()->AddGameObject<FloorDecision>(GetThis<MoveFloor>());

		try
		{	// objectの取得
			m_port = GetStage()->GetSharedGameObject<Port>(L"Port");
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		}
		catch (...) {
			m_port.reset();
		}
	}

	void MoveFloor::OnUpdate()
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
			//移動前の位置を覚えておく
			Vec3 oldPos = pos;

			switch (m_moveAxis)
			{
			case MoveAxis::X:
				newPos.x += m_speed * delta;
				if (newPos.x > m_pos.x + m_limitDist || newPos.x < m_pos.x - m_limitDist)
				{
					m_speed *= -1.0f;
				}

				break;
			case MoveAxis::Y:
				newPos.y += m_speed * delta;
				if (newPos.y > m_pos.y + m_limitDist || newPos.y < m_pos.y - m_limitDist)
				{
					m_speed *= -1.0f;
				}

				break;
			case MoveAxis::Z:
				newPos.z += m_speed * delta;
				if (newPos.z > m_pos.z + m_limitDist || newPos.z < m_pos.z - m_limitDist)
				{
					m_speed *= -1.0f;
				}

				break;
			}

			m_transform->SetPosition(newPos);

			Vec3 moveDelta = newPos - oldPos;
			//計算した移動量をFloorDecisionに渡す
			m_floorDec->SetCurrentMoveVec(moveDelta);
		}
		else
		{
			m_isUp = false;
		}
	}
}
//end basecross
