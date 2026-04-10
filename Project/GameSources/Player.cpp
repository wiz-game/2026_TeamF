/*!
@file Player.cpp
@brief �v���C���[�Ȃǎ���
*/

#include "stdafx.h"
#include "Project.h"
#include "MainCamera.h"
#include "game_controller.h"
#include "CharacterController.h"

namespace basecross{
	// �v���C���[�̏����ݒ�
	void Player::OnCreate()
	{
		GetStage()->SetSharedGameObject(L"Player", GetThis<Player>());
		// �g�����X�t�H�[���R���|�[�l���g���擾���Ă���
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 0.05f, 0.0f));
		// �h���[�R���|�[�l���g��ǉ�
		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SPHERE");
		m_draw->SetDiffuse(Col4(1, 1, 1, 1));

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

	// �v���C���[�̍X�V����
	void Player::OnUpdate()
	{
		//// �A�v���P�[�V�����I�u�W�F�N�g���擾
		//auto& app = App::GetApp();

		//// �O�񂩂�̌o�ߎ��ԁF�f���^�^�C�����擾����
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
	}

	void Player::OnMove()
	{
		auto& app = App::GetApp();
		std::wstringstream wss(L"");
		//auto cc = GetComponent<CharacterController>();
		
		//�R���g���[���[�擾
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		Vec3 stickL(pad.fThumbLX, 0.0f, pad.fThumbLY);

		auto key = device.GetKeyState();

		//ElapsedTime�擾
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
				auto ink = stage->AddGameObject<InkDraw>();
				ink->GetComponent<Transform>()->SetPosition(Vec3(m_pos.x, m_pos.y - m_height / 2, m_pos.z));
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
			wss << L"\n" << L"Died!!";
		}
		app->GetScene<Scene>()->SetDebugString(wss.str());
	}

	void Player::UpdateMoveFloor()
	{
		auto scene = App::GetApp()->GetScene<Scene>();
		auto cc = GetComponent<CharacterController>();
		if (!cc || !m_currentFloor) return;

		//���������Ă���(isUp)���ڒn���Ă��邩�`���b�N
		auto shouldBeParent = cc->IsOnGround() && m_currentFloor->GetIsUp();
		if (shouldBeParent)
		{
			cc->SetGravityEnabled(false); //�d�͂𖳌��ɂ���

			//float delta = App::GetApp()->GetElapsedTime();
			
			//���̈ړ��ʂ��v�Z����Player�̍��W�ɉ��Z
			float floorVelocityY = m_currentFloor->GetMoveSpeed();

			Vec3 currentV = cc->GetLinearVelocity();
			currentV.y = floorVelocityY; //���̈ړ��ʂ��v���C���[�̑��x�ɉ��Z
			cc->SetLinearVelocity(currentV);

			//�f�o�b�O�����̕\��
			std::wstring debugMsg = L"Grounded: " + std::wstring(cc->IsOnGround() ? L"true" : L"false")
				+ L" | IsUp: " + (m_currentFloor->GetIsUp() ? L"true" : L"false")
				+ L"\n"
				+ L"���ړ����BspeedY: " + std::to_wstring(floorVelocityY);
			scene->SetDebugString(debugMsg);
		}
		else
		{
			cc->SetGravityEnabled(true); //�d�͂�L���ɂ���
		}
	}

	// ���Ƃ̏ՓˊJ�n
	void Player::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		if (floor)
		{
			m_currentFloor = floor;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"���ɏ��܂���");
		}
	}

	// ���Ƃ̏Փˌp��
	void Player::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		if (floor)
		{
			m_currentFloor = floor;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"���ɏ��܂���");
		}

	}

	// ���Ƃ̏ՓˏI��
	void Player::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		auto floor = dynamic_pointer_cast<UpDownFloor>(obj);
		if (floor)
		{
			m_currentFloor = nullptr;
			auto scene = App::GetApp()->GetScene<Scene>();
			//scene->SetDebugString(L"������~��܂���");
		}
	}
}
//end basecross

