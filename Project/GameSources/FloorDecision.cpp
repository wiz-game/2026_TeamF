/*!
@file FloorDecision.cpp
@brief MoveFloorの床の判定
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void FloorDecision::OnCreate()
	{
		m_transform = GetComponent<Transform>();

		//親のscaleを取得して自動計算
		if (m_owner)
		{
			auto ownerScale = m_owner->GetComponent<Transform>()->GetScale();
			//各軸から0.5小さくする（マイナスにならないようにmaxでガードする）
			m_scale.x = (std::max)(0.1f, ownerScale.x - 0.5f);
			m_scale.y = 0.1f;//固定
			m_scale.z = (std::max)(0.1f, ownerScale.z - 0.5f);
		}
		m_transform->SetParent(m_owner);
		m_transform->SetScale(m_scale);
		m_transform->SetPosition(0, 0.05f, 0);

		m_coll = AddComponent<CollisionObb>();
		m_coll->SetAfterCollision(AfterCollision::None);
		m_coll->SetDrawActive(false);
		m_coll->SetFixed(false);

		m_lastPosition = m_transform->GetWorldPosition();
	}

	void FloorDecision::OnUpdate()
	{
		//現在の世界座標を取得し、前フレームからの移動差分を計算
		Vec3 currentWorldPos = m_transform->GetWorldPosition();
		m_currentMoveVec = currentWorldPos - m_lastPosition;
		m_lastPosition = currentWorldPos;//次フレームのために保存
	}

	void FloorDecision::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = dynamic_pointer_cast<Player>(obj))
		{
			//Playerに移動量を渡す
			player->UpdateMoveFloor(m_currentMoveVec);
			m_isPlayerOn = true;
		}
		if (auto moveObj = dynamic_pointer_cast<MoveObj>(obj))
		{
			//Playerに移動量を渡す
			moveObj->UpdateMoveFloor(m_currentMoveVec);
			m_isPlayerOn = true;
		}

	}

	void FloorDecision::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		OnCollisionEnter(obj);//Enterと同じ処理を行う
	}
	void FloorDecision::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = dynamic_pointer_cast<Player>(obj))
		{
			player->UpdateMoveFloor(Vec3(0));
			m_isPlayerOn = false;
		}
	}
}
//end basecross
