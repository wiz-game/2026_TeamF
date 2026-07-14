/*!
@file TrapDoorAxis.cpp
@brief トラップドアの実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void TrapDoorAxis::OnCreate()
	{
		m_trans = GetComponent<Transform>();
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);

		//m_staticDraw = AddComponent<PNTStaticDraw>();
		//m_staticDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//auto coll = AddComponent<CollisionObb>();
		//coll->SetDrawActive(true);

		//初期のRotationを決めるためのOffsetObjectを作成
		auto offsetObj = GetStage()->AddGameObject<OffsetObject>(GetThis<TrapDoorAxis>());
		auto offsetTrans = offsetObj->GetComponent<Transform>();
		offsetTrans->SetRotation(m_initialRotation);

		//TrapDoorの作成と位置をずらすためのObject
		m_trapDoor = GetStage()->AddGameObject<TrapDoor>(offsetObj, m_trapDoorScale);
		
		//X軸回転かつスピードがマイナスの時は
		if (m_moveAxis == MoveAxis::X && m_speed < 0)
		{
			m_trapDoor->GetComponent<Transform>()->SetRotation(Vec3(0,XM_PI,0));//オブジェクトのYを180度回転させる
		}
	}

	void TrapDoorAxis::OnUpdate()
	{
		if (m_port)
		{
			isConnect = m_port->GetConnect();
		}
		else
		{
			isConnect = true;
		}

		auto scene = App::GetApp()->GetScene<Scene>();
		float delta = App::GetApp()->GetElapsedTime();

		const float LIMIT_ANGLE = (m_speed > 0) ? XM_PIDIV2 : -XM_PIDIV2; //90度の制限

		//通電していれば動く
		if (isConnect)
		{
			m_isMove = true;
			m_trapDoor->SetIsMove(true);
			switch (m_moveAxis)
			{
			case MoveAxis::X:
			{
				bool canMove = (m_speed > 0) ? (m_rotateVec.x < LIMIT_ANGLE) : (m_rotateVec.x > LIMIT_ANGLE);
				if (canMove)
				{
					m_rotateVec.x += m_speed;
					// 制限角度を超えないようにする
					if (m_speed > 0 && m_rotateVec.x > LIMIT_ANGLE) m_rotateVec.x = LIMIT_ANGLE;
					if (m_speed < 0 && m_rotateVec.x < LIMIT_ANGLE) m_rotateVec.x = LIMIT_ANGLE;
				}

				break;
			}
			case MoveAxis::Y:
			{
				bool canMoveY = (m_speed > 0) ? (m_rotateVec.y < LIMIT_ANGLE) : (m_rotateVec.y > LIMIT_ANGLE);
				if (canMoveY)
				{
					m_rotateVec.y += m_speed;
					if (m_speed > 0 && m_rotateVec.y > LIMIT_ANGLE) m_rotateVec.y = LIMIT_ANGLE;
					if (m_speed < 0 && m_rotateVec.y < LIMIT_ANGLE) m_rotateVec.y = LIMIT_ANGLE;

				}

				break;
			}
			case MoveAxis::Z:
			{
				bool canMoveZ = (m_speed > 0) ? (m_rotateVec.z < LIMIT_ANGLE) : (m_rotateVec.z > LIMIT_ANGLE);
				if (canMoveZ)
				{
					m_rotateVec.z += m_speed;
					if (m_speed > 0 && m_rotateVec.z > LIMIT_ANGLE) m_rotateVec.z = LIMIT_ANGLE;
					if (m_speed < 0 && m_rotateVec.z < LIMIT_ANGLE) m_rotateVec.z = LIMIT_ANGLE;
				}

				break;
			}
			}

			m_trans->SetRotation(m_rotateVec);
		}
		else
		{
			// 0に戻る処理（正負どちらからでも0に近づける）
			if (std::abs(m_rotateVec.x) > 0.001f) {
				float returnDir = (m_rotateVec.x > 0) ? -1.0f : 1.0f;
				m_rotateVec.x += std::abs(m_speed) * returnDir;
				if ((returnDir < 0 && m_rotateVec.x < 0) || (returnDir > 0 && m_rotateVec.x > 0)) {
					m_rotateVec.x = 0;
				}
			}

			if (std::abs(m_rotateVec.y) > 0.001f) {
				float returnDir = (m_rotateVec.y > 0) ? -1.0f : 1.0f;
				m_rotateVec.y += std::abs(m_speed) * returnDir;
				if ((returnDir < 0 && m_rotateVec.y < 0) || (returnDir > 0 && m_rotateVec.y > 0)) {
					m_rotateVec.y = 0;
				}
			}

			if (std::abs(m_rotateVec.z) > 0.001f) {
				float returnDir = (m_rotateVec.z > 0) ? -1.0f : 1.0f;
				m_rotateVec.z += std::abs(m_speed) * returnDir;
				if ((returnDir < 0 && m_rotateVec.z < 0) || (returnDir > 0 && m_rotateVec.z > 0)) {
					m_rotateVec.z = 0;
				}
			}

			m_isMove = false;
			m_trapDoor->SetIsMove(false);

			m_trans->SetRotation(m_rotateVec);
		}
	}
}
//end basecross
