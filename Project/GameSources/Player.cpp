/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "CharacterController.h"

namespace basecross{
	// プレイヤーの初期設定
	void Player::OnCreate()
	{
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 5.0f, 0.0f));
		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SPHERE");
		m_draw->SetDiffuse(Col4(1, 1, 1, 1));

		auto cc = AddComponent<CharacterController>();
		CharacterController::Settings settings;
		settings.height = m_height;
		settings.radius = m_radius;
		settings.mass = 0.0f;
		settings.maxSlopeAngle - 45.0f;
		cc->Initialize(settings);

		m_ink = m_inkMax;
	}

	// プレイヤーの更新処理
	void Player::OnUpdate()
	{
		m_pos = m_transform->GetPosition();
		//// アプリケーションオブジェクトを取得
		//auto& app = App::GetApp();

		//// 前回からの経過時間：デルタタイムを取得する
		//float delta = app->GetElapsedTime();

		//auto sensor = GameController::GetSensorData();
		//Vec3 gyro = Vec3(sensor.gyroX, sensor.gyroY, sensor.gyroZ);

		//auto quat = m_transform->GetQuaternion();
		////auto rotZ = (Quat)XMQuaternionRotationAxis(Vec3(1, 0, 0), gyro.z);
		//auto rotY = (Quat)XMQuaternionRotationAxis(Vec3(0, 1, 0), gyro.y);
		////auto rotX = (Quat)XMQuaternionRotationAxis(Vec3(0, 0, 1), gyro.x);

		////quat = quat * rotX * rotY * rotZ;
		//quat = quat * rotY;
		//m_transform->SetQuaternion(rotY);
		OnMove();
		DropInk();
	}

	void Player::OnMove()
	{
		auto& app = App::GetApp();
		auto delta = app->GetElapsedTime();
		auto cc = GetComponent<CharacterController>();

		auto pad = GameController::GetCurrentState();
		auto LStickX = GameController::GetLeftStickX();
		auto LStickY = GameController::GetLeftStickY();
		const Vec3 stickL(LStickX, 0.0f, -LStickY);

		if (cc)
		{
			m_moveDir = stickL;
			m_moveDir.normalize();
			m_moveSpeed = m_maxSpeed * stickL.length();
			float rad = atan2f(m_moveDir.z, m_moveDir.x);

			m_moveDir.x = cosf(rad);
			m_moveDir.z = sinf(rad);

			cc->SetLinearVelocity(m_moveSpeed * m_moveDir);
		}
	}

	void Player::DropInk()
	{
		auto delta = App::GetApp()->GetElapsedTime();
		auto pad = GameController::GetCurrentState();
		auto cc = GetComponent<CharacterController>();
		auto stage = GetStage();

		if (pad.buttonDown)
		{
			if (cc && cc->IsOnGround())
			{
				m_ink -= delta;
				auto ink = stage->AddGameObject<InkDraw>();
				ink->GetComponent<Transform>()->SetPosition(Vec3(m_pos.x, m_pos.y - m_height / 2, m_pos.z));
			}
		}
	}
}
//end basecross

