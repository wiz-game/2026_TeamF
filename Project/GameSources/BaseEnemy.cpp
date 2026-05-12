/*!
@file BaseEnemy.cpp
@brief キャラクターなど実体
Project.hに対応する全てのヘッダをまとめている
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void BaseEnemy::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(1, 1, 1);

		m_state = State::Idle;
	}

	void BaseEnemy::OnUpdate()
	{
		switch (m_state) {
		case State::Idle:
			UpdateIdle();
			break;
		case State::Move:
			UpdateMove();
			break;
		case State::Drow:
			UpdateInkDrow();
			break;
		case State::Erase:
			UpdateInkErase();
			break;
		}
	}

	void BaseEnemy::UpdateIdle()
	{

	}

	void BaseEnemy::UpdateMove()
	{

	}

	void BaseEnemy::UpdateInkDrow()
	{
		Vec3 pos = m_transform->GetPosition();
		auto delta = App::GetApp()->GetElapsedTime();
		pos.x += 1.0 * delta;
		m_transform->SetPosition(pos);
	}

	void BaseEnemy::UpdateInkErase()
	{
		Vec3 pos = m_transform->GetPosition();
		auto delta = App::GetApp()->GetElapsedTime();
		pos.y += 1.0 * delta;
		m_transform->SetPosition(pos);

	}
}
//end basecross
