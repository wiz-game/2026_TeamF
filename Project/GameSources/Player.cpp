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
		GetStage()->SetSharedGameObject(L"Player", GetThis<Player>());
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

		auto coll = AddComponent<CollisionObb>();
		//coll->SetAfterCollision(AfterCollision::None);
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
		UpdateMoveFloor();
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

	void Player::UpdateMoveFloor()
	{
		auto scene = App::GetApp()->GetScene<Scene>();
		auto cc = GetComponent<CharacterController>();
		if (!cc || !m_currentFloor) return;

		//床が動いている(isUp)かつ接地しているかチャック
		auto shouldBeParent = cc->IsOnGround() && m_currentFloor->GetIsUp();
		if (shouldBeParent)
		{
			cc->SetGravityEnabled(false); //重力を無効にする

			//float delta = App::GetApp()->GetElapsedTime();
			
			//床の移動量を計算してPlayerの座標に加算
			float floorVelocityY = m_currentFloor->GetMoveSpeed();

			Vec3 currentV = cc->GetLinearVelocity();
			currentV.y = floorVelocityY; //床の移動量をプレイヤーの速度に加算
			cc->SetLinearVelocity(currentV);

			//デバッグ文字の表示
			std::wstring debugMsg = L"Grounded: " + std::wstring(cc->IsOnGround() ? L"true" : L"false")
				+ L" | IsUp: " + (m_currentFloor->GetIsUp() ? L"true" : L"false")
				+ L"\n"
				+ L"床移動中。speedY: " + std::to_wstring(floorVelocityY);
			scene->SetDebugString(debugMsg);
		}
		else
		{
			cc->SetGravityEnabled(true); //重力を有効にする
		}
	}

	// 床との衝突開始
	void Player::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		if (floor)
		{
			m_currentFloor = floor;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"床に乗りました");
		}
	}

	// 床との衝突継続
	void Player::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		if (floor)
		{
			m_currentFloor = floor;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"床に乗りました");
		}

	}

	// 床との衝突終了
	void Player::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		if (floor)
		{
			m_currentFloor = nullptr;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"床から降りました");
		}
	}
}
//end basecross

