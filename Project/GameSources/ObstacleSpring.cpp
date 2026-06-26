/*!
@file ObstacleSpring.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//初期化
	void ObstacleSpring::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CYLINDER");
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(1, 2, 1);
		m_transform->SetPosition(0, 1, 0);
		m_transform->SetRotation(0, 0, XM_PIDIV2);
		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CYLINDER");

		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Spring", path + L"Spring.png");
		m_draw->SetTextureResource(L"Spring");
	}

	void ObstacleSpring::OnUpdate()
	{
		float delta = App::GetApp()->GetElapsedTime();
		Roteto(delta);
		UpdateGravity(delta);
	}

	void ObstacleSpring::Roteto(float delta)
	{
		m_angle += m_roteto * delta;

		m_transform->SetRotation(m_angle, 0, XM_PIDIV2);

		if (m_angle > XM_2PI) {
			m_angle -= XM_2PI;
		}

		//Vec3 forward = m_transform->GetForward();
		Vec3 pos = m_transform->GetPosition();
		pos.z +=  m_moveSpeed * delta;

		m_transform->SetPosition(pos);
	}


	void ObstacleSpring::PushPlayer(std::shared_ptr<Player> player)
	{

		Vec3 myPos = m_transform->GetPosition();
		Vec3 playerPos = player->GetComponent<Transform>()->GetPosition();

		Vec3 dir = playerPos - myPos;
		dir.y = 0.0f;

		if (dir.length() > 0.001f)
		{
			dir.normalize();
		}

		float power = 5.0f;

		player->AddForce(dir * power);

	}


	void ObstacleSpring::UpdateGravity(float delta)
	{
		Vec3 pos = m_transform->GetPosition();

		m_velocity += m_gravity * delta;

		pos.y += m_velocity * delta;


		if (pos.y <= m_floor)
		{
			pos.y = m_floor;
			m_velocity = 0.0f;
			m_isGround = true;
		}
		else
		{
			m_isGround = false;
		}

		m_transform->SetPosition(pos);
	}

	void ObstacleSpring::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto floor = dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = true;
			m_velocity = 0.0f;
		}
	}


	void ObstacleSpring::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = dynamic_pointer_cast<Player>(obj))
		{
			PushPlayer(player);
		}
	}


	void ObstacleSpring::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = false;
		}
	}

}
//end basecross
