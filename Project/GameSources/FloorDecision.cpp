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
		m_transform->SetScale(m_scale);
		m_transform->SetParent(m_owner);
		m_transform->SetPosition(0, 0.1f, 0);

		m_coll = AddComponent<CollisionObb>();
		m_coll->SetAfterCollision(AfterCollision::None);
		m_coll->SetDrawActive(false);
		m_coll->SetFixed(false);

	}

	void FloorDecision::OnUpdate()
	{

	}

	void FloorDecision::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = dynamic_pointer_cast<Player>(obj))
		{
			//Playerに移動量を渡す
			player->UpdateMoveFloor(m_currentMoveVec);
		}
	}

	void FloorDecision::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		OnCollisionEnter(obj);//Enterと同じ処理を行う
	}
	void FloorDecision::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		//if (auto player = dynamic_pointer_cast<Player>(obj))
		//{
		//	player->UpdateMoveFloor(Vec3(0));
		//}
	}
}
//end basecross
