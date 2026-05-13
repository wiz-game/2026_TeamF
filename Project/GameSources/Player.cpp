/*!
@file Player.cpp
@brief �ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾈど趣ｿｽ�ｽ�ｽ
*/

#include "stdafx.h"
#include "Project.h"
#include "MainCamera.h"
#include "game_controller.h"
#include "CharacterController.h"

namespace basecross{
	Player::Player
	(
		const std::shared_ptr<Stage>& stage,
		const Vec3& Scale,
		const Vec3& Rot,
		const Vec3& Position
	) :
		StageObjBase(stage, Scale, Rot, Position),
		m_height(1.0f),
		m_radius(0.49f),
		m_moveSpeed(0.0f),
		m_maxSpeed(5.0f),
		m_accel(0.99f),
		m_pos(Position),
		m_forward(Vec3(0)),
		m_velocity(Vec3(0)),
		m_ink(0.0f),
		m_inkMax(10.0f),
		m_inkDecrease(0.9f),
		m_isDraw(true),
		m_fade(0.0f),
		m_externalVelocity(Vec3(0)),
		m_gravity(-9.8f),
		m_isGround(false)
	{
	}

	Player::~Player()
	{
	}
	// �ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾌ擾ｿｽ�ｽ�ｽ�ｽﾝ抵ｿｽ
	void Player::OnCreate()
	{
		GetStage()->SetSharedGameObject(L"Player", GetThis<Player>());
		// �ｽg�ｽ�ｽ�ｽ�ｽ�ｽX�ｽt�ｽH�ｽ[�ｽ�ｽ�ｽR�ｽ�ｽ�ｽ|�ｽ[�ｽl�ｽ�ｽ�ｽg�ｽ�ｽ�ｽ謫ｾ�ｽ�ｽ�ｽﾄゑｿｽ�ｽ�ｽ
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 0.05f, 0.0f));
		// �ｽh�ｽ�ｽ�ｽ[�ｽR�ｽ�ｽ�ｽ|�ｽ[�ｽl�ｽ�ｽ�ｽg�ｽ�ｽﾇ会ｿｽ
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SPHERE");

		SetAlphaActive(true);
		//m_draw->SetDiffuse(Col4(0, 0, 0, 1.0f));
		m_draw->SetEmissive(Col4(0, 0, 0, 1.0f));

		auto cc = AddComponent<CharacterController>();
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
		auto coll = AddComponent<CollisionObb>();
	}

	// �ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾌ更�ｽV�ｽ�ｽ�ｽ�ｽ
	void Player::OnUpdate()
	{
		//// �ｽA�ｽv�ｽ�ｽ�ｽP�ｽ[�ｽV�ｽ�ｽ�ｽ�ｽ�ｽI�ｽu�ｽW�ｽF�ｽN�ｽg�ｽ�ｽ�ｽ謫ｾ
		m_pos = m_transform->GetPosition();
		auto scene = App::GetApp()->GetScene<Scene>();
		//// �ｽA�ｽv�ｽ�ｽ�ｽP�ｽ[�ｽV�ｽ�ｽ�ｽ�ｽ�ｽI�ｽu�ｽW�ｽF�ｽN�ｽg�ｽ�ｽ�ｽ謫ｾ
		//auto& app = App::GetApp();

		//// �ｽO�ｽｩゑｿｽﾌ経�ｽﾟ趣ｿｽ�ｽﾔ：�ｽf�ｽ�ｽ�ｽ^�ｽ^�ｽC�ｽ�ｽ�ｽ�ｽ�ｽ謫ｾ�ｽ�ｽ�ｽ�ｽ
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

		if (m_pos.y <= -10.0f)
		{
			PostEvent(0.0f, GetThis<Player>(), scene, L"ToProtoStage");
		}

		scene->SetDebugString(L"PlayerPos:" + std::to_wstring(m_pos.x) + L", " + std::to_wstring(m_pos.y) + L", " + std::to_wstring(m_pos.z)
			+ L"\n"
			+ L"ink残量 : " + std::to_wstring(m_ink)
			+ L"\n"
			+ L"isGround : " + (m_isGround ? L"true" : L"false")
			+ L"\n"
			+ L" m_FloorDecision : " + (m_floorDecision ? L"Valid" : L"null"));
	}

	void Player::OnMove()
	{
		auto& app = App::GetApp();
		std::wstringstream wss(L"");
		//auto cc = GetComponent<CharacterController>();
		
		//�ｽR�ｽ�ｽ�ｽg�ｽ�ｽ�ｽ[�ｽ�ｽ�ｽ[�ｽ謫ｾ
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		Vec3 stickL(pad.fThumbLX, 0.0f, pad.fThumbLY);

		auto key = device.GetKeyState();

		//ElapsedTime�ｽ謫ｾ
		auto delta = app->GetElapsedTime();

		m_pos = m_transform->GetPosition();

		float cameraAngleY = 0.0f;
		auto mainCamera = m_camera.lock();
		if (mainCamera)
		{
			cameraAngleY = mainCamera->GetAngleY();
		}

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

		if (m_isGround)
		{
			m_velocity.y = 0.0f;
		}
		else
		{
			m_velocity.y += m_gravity * delta;
		}
		
		if (m_velocity.x <= m_maxSpeed || m_velocity.z <= m_maxSpeed)
			m_velocity *= m_accel;
		m_pos.x += m_moveSpeed * m_velocity.x * delta;
		m_pos.z += m_moveSpeed * m_velocity.z * delta;
		m_pos.y += m_velocity.y * delta;	//重力による落下

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
			if (m_isDraw && m_ink > 0 && m_isGround)
			{
				m_ink -= m_inkDecrease * delta;
				m_ink -= delta;

				std::vector<std::shared_ptr<GameObject>> objVec;
				GetStage()->GetUsedTagObjectVec(L"InkCloud", objVec);

				m_targetCloud = nullptr;

				for (auto& obj : objVec)
				{
					auto cloud = std::dynamic_pointer_cast<InkCloud>(obj);
					if (cloud)
					{
						float dist = (cloud->GetComponent<Transform>()->GetPosition() - m_pos).length();

						if (dist < 1.0f)
						{
							m_targetCloud = cloud;
							break;
						}
					}
				}

				if (!m_targetCloud)
				{
					m_targetCloud = GetStage()->AddGameObject<InkCloud>();
				}

				auto ink = stage->AddGameObject<InkDraw>();
				ink->FadingInk(m_fade);
				m_fade += (1.5f * m_inkDecrease) * delta / m_inkMax;
				ink->GetComponent<Transform>()->SetPosition(Vec3(m_pos.x, m_pos.y - m_height / 3, m_pos.z));
				m_targetCloud->AddInk(ink);//インクを追加
			}
		}
	}

	void Player::OnDied()
	{
		auto& app = App::GetApp();
		std::wstringstream wss(L"");

		wss << m_ink;

		if (m_ink <= 0)
		{
			wss << L"\n" << L"Died!!\n";
		}
		app->GetScene<Scene>()->SetDebugString(wss.str());
	}

	//移動量を取得する関数
	void Player::UpdateMoveFloor(const Vec3& movePos)
	{
		// m_floorDecision が null なら何もしない
		if (!m_floorDecision) return;

		m_pos += movePos; //Playerに床の移動量を加算
		m_transform->SetPosition(m_pos);
	}

	// 衝突開始
	void Player::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto floor = dynamic_pointer_cast<FloorDecision>(obj))
		{
			m_floorDecision = floor;
			m_isGround = true;
			m_isDraw = false;
		}

		auto ink = dynamic_pointer_cast<InkDraw>(obj);
		if (ink)
		{
			m_isDraw = false;
		}

		if (auto floor = dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = true;
		}

		if (auto trapDoor = dynamic_pointer_cast<TrapDoor>(obj))
		{
			if (trapDoor->GetIsMove())
			{
				m_isGround = true;
			}
			else
			{
				m_isGround = false;
			}
		}
	}

	//衝突中
	void Player::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		if (auto floor = dynamic_pointer_cast<FloorDecision>(obj))
		{
			m_floorDecision = floor;
			m_isGround = true;
			m_isDraw = false;
		}

		// 接触が継続していれば接地フラグを維持
		if (dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = true;
		}

		auto ink = dynamic_pointer_cast<InkDraw>(obj);
		if (ink)
		{
			m_isDraw = false;
		}

		if (auto trapDoor = dynamic_pointer_cast<TrapDoor>(obj))
		{
			if (trapDoor->GetIsMove())
			{
				m_isGround = true;
			}
			else
			{
				m_isGround = false;
			}
		}

	}

	//衝突終了
	void Player::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		auto ink = dynamic_pointer_cast<InkDraw>(obj);
		if (ink)
		{
			m_isDraw = true;
		}

		if (dynamic_pointer_cast<FloorDecision>(obj))
		{
			m_floorDecision = nullptr;
			m_isGround = false;
			m_isDraw = true;
		}

		if (dynamic_pointer_cast<Floor>(obj))
		{
			m_isGround = false;
		}

		if (auto trapDoor = dynamic_pointer_cast<TrapDoor>(obj))
		{
			m_isGround = false;
		}

	}
}
//end basecross

