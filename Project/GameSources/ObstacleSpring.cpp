/*!
@file ObstacleSpring.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	// ===== 定数まとめ =====
	const Vec3 ObstacleSpring::SCALE = Vec3(1.0f, 2.0f, 1.0f);
	const Vec3 ObstacleSpring::START_POS = Vec3(0.0f, 5.0f, 0.0f);
	const float ObstacleSpring::ROTATION_Z = XM_PIDIV2;
	const float ObstacleSpring::PUSH_POWER = 0.3f;
	const float ObstacleSpring::NORMALIZE_EPS = 0.001f;
	const float ObstacleSpring::LOOP_ANGLE = XM_2PI;
	const float ObstacleSpring::DEAD_LINE = -10.0f;

	void ObstacleSpring::OnCreate()
	{
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CYLINDER");
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(SCALE);
		m_transform->SetPosition(START_POS);
		m_transform->SetRotation(0, 0, ROTATION_Z);
		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CYLINDER");

		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Spring", path + L"Spring.png");
		m_draw->SetTextureResource(L"Spring");
		auto coll = AddComponent<CollisionObb>();

	}

	void ObstacleSpring::OnUpdate()
	{
		float delta = App::GetApp()->GetElapsedTime();
		Roteto(delta);
		UpdateGravity(delta);

		Vec3 pos = m_transform->GetPosition();
		if (pos.y < DEAD_LINE)
		{
			OnDestroy();
		}
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

		if (dir.length() > NORMALIZE_EPS)
		{
			dir.normalize();
		}
    
		player->AddVelocity(dir * PUSH_POWER);

	}


	void ObstacleSpring::UpdateGravity(float delta)
	{
		Vec3 pos = m_transform->GetPosition();

		m_velocity += m_gravity * delta;

		pos.y += m_velocity * delta;


		if (!m_isGround)
		{
			m_gravity = -9.8f;
			m_velocity += m_gravity * delta;
			pos.y += m_velocity * delta;
		}
		else
		{
			m_velocity = 0.0f;
			m_gravity = 0.0f;
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

		if (auto player = std::dynamic_pointer_cast<Player>(obj))
		{
			PushPlayer(player);
			SoundManager::Get().PlaySE(L"STEELHIT", 0.1f);
		}

	}


	void ObstacleSpring::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		if (auto player = std::dynamic_pointer_cast<Player>(obj))
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
