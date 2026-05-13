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
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		}
		catch (...) {
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
			//移動前の位置を覚えておく
			Vec3 oldPos = pos;
			if (m_floorDec->GetIsPlayerOn())//プレイヤーが乗っているならば床を動かす
			{
				m_isUp = true;

				switch (m_moveAxis)
				{
				case MoveAxis::X:
					newPos.x += m_speed * delta;
					if (newPos.x > m_pos.x + m_limitDist)
					{
						newPos.x = m_pos.x + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
					}
					else if (newPos.x < m_pos.x)
					{
						newPos.x = m_pos.x; // 強制的に限界地へ
						m_speed *= -1.0f;
					}

					break;
				case MoveAxis::Y:
					newPos.y += m_speed * delta;
					if (newPos.y > m_pos.y + m_limitDist)
					{
						newPos.y = m_pos.y + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
					}
					else if (newPos.y < m_pos.y)
					{
						newPos.y = m_pos.y; // 強制的に限界地へ
						m_speed *= -1.0f;
					}

					break;
				case MoveAxis::Z:
					newPos.z += m_speed * delta;
					if (newPos.z > m_pos.z + m_limitDist)
					{
						newPos.z = m_pos.z + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
					}
					else if (newPos.z < m_pos.z)
					{
						newPos.z = m_pos.z; // 強制的に限界地へ
						m_speed *= -1.0f;
					}

					break;
				}
				m_transform->SetPosition(newPos);

				Vec3 moveDelta = newPos - oldPos;
				//計算した移動量をFloorDecisionに渡す
				m_floorDec->SetCurrentMoveVec(moveDelta);
			}
			else//プレイヤーが乗っていないならば床を元の位置に戻す
			{
				m_isUp = false;
				//床を元の位置に戻すための速度を計算する
				float returnSpeed = fabsf(m_speed) * 0.5f;
				switch (m_moveAxis)
				{
				case MoveAxis::X:
					if (newPos.x > m_pos.x) {
						newPos.x -= returnSpeed * delta;
						if (newPos.x < m_pos.x) newPos.x = m_pos.x;
					}
					break;
				case MoveAxis::Y:
					if (newPos.y > m_pos.y) {
						newPos.y -= returnSpeed * delta;
						if (newPos.y < m_pos.y) newPos.y = m_pos.y;
					}
					break;
				case MoveAxis::Z:
					if (newPos.z > m_pos.z) {
						newPos.z -= returnSpeed * delta;
						if (newPos.z < m_pos.z) newPos.z = m_pos.z;
					}
					break;
				}		

				m_speed = fabs(m_speed); // 速度を正の値にする
				m_transform->SetPosition(newPos);

				//Vec3 moveDelta = newPos - oldPos;
				//計算した移動量をFloorDecisionに渡す
				//m_floorDec->SetCurrentMoveVec(moveDelta);
			}
		}
		else
		{
			m_isUp = false;
			m_floorDec->SetCurrentMoveVec(Vec3(0, 0, 0)); // 通電していないときは移動量をゼロにする
		}
	}
}
//end basecross
