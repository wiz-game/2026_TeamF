/*!
@file FloorDecision.cpp
@brief MoveFloorの床の判定
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void OffsetObject::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(m_scale);
		m_transform->SetParent(m_owner);
		m_transform->SetPosition(0, 0.1f, 0);
	}

	void OffsetObject::OnUpdate()
	{

	}

}
//end basecross
