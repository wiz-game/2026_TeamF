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
		m_draw->SetDiffuse(Col4(0, 0, 0, 1));
		m_draw->SetEmissive(Col4(0, 0, 0, 1));

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
		//coll->SetAfterCollision(AfterCollision::None);
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
		UpdateMoveFloor();

		if (m_pos.y <= -10.0f)
		{
			PostEvent(0.0f, GetThis<Player>(), scene, L"ToProtoStage");
		}

		scene->SetDebugString(L"PlayerPos:" + std::to_wstring(m_pos.x) + L", " + std::to_wstring(m_pos.y) + L", " + std::to_wstring(m_pos.z)
			+ L"\n"
			+ L"ink残量 : " + std::to_wstring(m_ink));

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

		
		if (m_velocity.x <= m_maxSpeed || m_velocity.z <= m_maxSpeed)
			m_velocity *= m_accel;
		m_pos += m_moveSpeed * m_velocity * delta;

		m_pos.y = 0.5f;
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
			if (m_ink > 0)
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

						if (dist < 2.0f)
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
				m_fade += (0.5f / m_inkDecrease) * delta / m_inkMax;
				ink->GetComponent<Transform>()->SetPosition(Vec3(m_pos.x, m_pos.y - m_height / 2, m_pos.z));
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

	void Player::UpdateMoveFloor()
	{
		auto scene = App::GetApp()->GetScene<Scene>();
		auto cc = GetComponent<CharacterController>();

		//�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾄゑｿｽ�ｽ�ｽ(isUp)�ｽ�ｽ�ｽﾂ接地�ｽ�ｽ�ｽﾄゑｿｽ�ｽ驍ｩ�ｽ`�ｽ�ｽ�ｽb�ｽN
		auto shouldBeParent = cc->IsOnGround() && m_currentFloor->GetIsUp();
		if (shouldBeParent)
		{
			cc->SetGravityEnabled(false); //�ｽd�ｽﾍを無鯉ｿｽ�ｽﾉゑｿｽ�ｽ�ｽ

			//float delta = App::GetApp()->GetElapsedTime();
			
			//�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽﾊゑｿｽ�ｽv�ｽZ�ｽ�ｽ�ｽ�ｽPlayer�ｽﾌ搾ｿｽ�ｽW�ｽﾉ会ｿｽ�ｽZ
			float floorVelocityY = m_currentFloor->GetMoveSpeed();

			Vec3 currentV = cc->GetLinearVelocity();
			currentV.y = floorVelocityY; //�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽﾊゑｿｽ�ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾌ托ｿｽ�ｽx�ｽﾉ会ｿｽ�ｽZ
			cc->SetLinearVelocity(currentV);

			//�ｽf�ｽo�ｽb�ｽO�ｽ�ｽ�ｽ�ｽ�ｽﾌ表�ｽ�ｽ
			std::wstring debugMsg = L"Grounded: " + std::wstring(cc->IsOnGround() ? L"true" : L"false")
				+ L" | IsUp: " + (m_currentFloor->GetIsUp() ? L"true" : L"false")
				+ L"\n"
				+ L"�ｽ�ｽ�ｽﾚ難ｿｽ�ｽ�ｽ�ｽBspeedY: " + std::to_wstring(floorVelocityY);
			scene->SetDebugString(debugMsg);
		}
		else
		{
			cc->SetGravityEnabled(true); //�ｽd�ｽﾍゑｿｽL�ｽ�ｽ�ｽﾉゑｿｽ�ｽ�ｽ
		}
		if (!m_currentFloor) return;
		//m_curretnFloor�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾄゑｿｽ�ｽﾂ擾ｿｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾄゑｿｽ�ｽ�ｽ(isUp)�ｽ�ｽ�ｽ`�ｽ�ｽ�ｽb�ｽN
		if (m_currentFloor->GetIsUp())
		{
			//�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽﾊゑｿｽ�ｽ謫ｾ
			float floorVelocityY = m_currentFloor->GetMoveSpeed();

			Vec3 currentV = m_transform->GetPosition();
			currentV.y = floorVelocityY; //�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽﾊゑｿｽ�ｽv�ｽ�ｽ�ｽC�ｽ�ｽ�ｽ[�ｽﾌ托ｿｽ�ｽx�ｽﾉ会ｿｽ�ｽZ
			m_transform->SetPosition(Vec3(currentV.x, currentV.y, currentV.z));

			//�ｽf�ｽo�ｽb�ｽO�ｽ�ｽ�ｽ�ｽ�ｽﾌ表�ｽ�ｽ
			std::wstring debugMsg = L" PlayerPos : " + std::to_wstring(currentV.x) + L", " + std::to_wstring(currentV.y) + L", " + std::to_wstring(currentV.z)
				+ L"\n"
				+ L"�ｽ�ｽ�ｽﾚ難ｿｽ�ｽ�ｽ�ｽBspeedY : " + std::to_wstring(floorVelocityY);
			scene->SetDebugString(debugMsg);
		}
	}

	// �ｽ�ｽ�ｽﾆの衝突開�ｽn
	void Player::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		auto ink = dynamic_pointer_cast<InkDraw>(obj);
		if (floor)
		{
			m_currentFloor = floor;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"�ｽ�ｽ�ｽﾉ擾ｿｽ�ｽﾜゑｿｽ�ｽ�ｽ");
		}
		if (ink)
		{
			m_isDraw = false;
		}
	}

	// �ｽ�ｽ�ｽﾆの衝突継�ｽ�ｽ
	void Player::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		auto ink = dynamic_pointer_cast<InkDraw>(obj);
		if (floor)
		{
			m_currentFloor = floor;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"�ｽ�ｽ�ｽﾉ擾ｿｽ�ｽﾜゑｿｽ�ｽ�ｽ");
		}
		if (ink)
		{
			m_isDraw = false;
		}

	}

	// �ｽ�ｽ�ｽﾆの衝突終�ｽ�ｽ
	void Player::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		auto ink = dynamic_pointer_cast<InkDraw>(obj);
		if (floor)
		{
			m_currentFloor = nullptr;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ~�ｽ�ｽﾜゑｿｽ�ｽ�ｽ");
		}
		if (ink)
		{
			m_isDraw = true;
		}
	}
}
//end basecross

