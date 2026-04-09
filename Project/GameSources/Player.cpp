/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "MainCamera.h"
#include "game_controller.h"
#include "CharacterController.h"

namespace basecross{
	// プレイヤーの初期設定
	void Player::OnCreate()
	{
		GetStage()->SetSharedGameObject(L"Player", GetThis<Player>());
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 0.05f, 0.0f));
		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SPHERE");
		m_draw->SetDiffuse(Col4(1, 1, 1, 1));

		//auto cc = AddComponent<CharacterController>();
		//CharacterController::Settings settings;
		//settings.height = m_height;
		//settings.radius = m_radius;
		//settings.mass = 0.0f;
		//settings.maxSlopeAngle - 45.0f;
		//cc->Initialize(settings);

		m_moveSpeed = m_maxSpeed;
		m_ink = m_inkMax;

		auto view = GetStage()->GetView();
		auto camera = view->GetTargetCamera();
		m_camera = dynamic_pointer_cast<MainCamera>(camera);
	}

	// プレイヤーの更新処理
	void Player::OnUpdate()
	{
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
		OnDied();
	}

	void Player::OnMove()
	{
		auto& app = App::GetApp();
		std::wstringstream wss(L"");
		//auto cc = GetComponent<CharacterController>();
		
		//コントローラー取得
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		Vec3 stickL(pad.fThumbLX, 0.0f, pad.fThumbLY);

		auto key = device.GetKeyState();

		//ElapsedTime取得
		auto delta = app->GetElapsedTime();

		m_pos = m_transform->GetPosition();

		float cameraAngleY = 0.0f;
		auto mainCamera = m_camera.lock();
		if (mainCamera)
		{
			cameraAngleY = mainCamera->GetAngleY();
		}

		Vec3 v = Vec3(0);
		if (key.m_bPushKeyTbl['A'])
		{
			stickL.x = -m_moveSpeed;
		}
		if (key.m_bPushKeyTbl['D'])
		{
			stickL.x = m_moveSpeed;
		}
		if (key.m_bPushKeyTbl['W'])
		{
			stickL.z = m_moveSpeed;
		}
		if (key.m_bPushKeyTbl['S'])
		{
			stickL.z = -m_moveSpeed;
		}

		float length = stickL.length();
		if (length != 0)
		{
			float padAngle = atan2f(stickL.z, stickL.x);
			float forwardAngle = padAngle + cameraAngleY + XM_PIDIV2;

			m_forward.x = cosf(forwardAngle);
			m_forward.y = 0.0f;
			m_forward.z = sinf(forwardAngle);

			m_velocity += m_forward * delta;
;			//cc->SetLinearVelocity(m_moveSpeed * m_velocity * m_moveDir);
		}

		
		if (m_velocity.x <= m_maxSpeed || m_velocity.z <= m_maxSpeed)
			m_velocity *= m_accel;
		m_pos += m_moveSpeed * m_velocity * delta;
		m_transform->SetPosition(m_pos);
	}

	void Player::DropInk()
	{
		auto delta = App::GetApp()->GetElapsedTime();
		auto device = App::GetApp()->GetInputDevice();
		auto pad = device.GetControlerVec()[0];
		//auto pad = GameController::GetCurrentState();
		auto key = device.GetKeyState();
		auto stage = GetStage();

		if (pad.wButtons & XINPUT_GAMEPAD_A || key.m_bPushKeyTbl[' '])
		{
			if(m_ink>0)
			m_ink -= m_inkDecrease * delta;

			auto ink = stage->AddGameObject<InkDraw>();
			ink->GetComponent<Transform>()->SetPosition(Vec3(m_pos.x, m_pos.y - m_height / 2, m_pos.z));
		}
	}

	void Player::OnDied()
	{
		auto& app = App::GetApp();
		std::wstringstream wss(L"");

		wss << m_ink;

		if (m_ink <= 0)
		{
			wss << L"\n" << L"Died!!";
		}
		app->GetScene<Scene>()->SetDebugString(wss.str());
	}
}
//end basecross

