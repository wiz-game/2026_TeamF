/*!
@file Player.h
@brief �L�����N�^�[�Ȃ�
*/

#pragma once
#include "stdafx.h"
#include "MainCamera.h"
#include "PNTDXModelDraw.h"

namespace basecross {

class UpDownFloor;

	// GameObject�N���X���p�������uPlayer�v�N���X���`
	class Player : public GameObject // GameObject�N���X�̌p���y�K�{�z
	{
		std::shared_ptr<Transform> m_transform; // �g�����X�t�H�[���͂悭�g���̂Ń����o�ɂ��Ă���
		std::shared_ptr<PNTStaticDraw> m_draw; // �h���[�R���|�[�l���g

		std::weak_ptr<MainCamera> m_camera;

		std::shared_ptr<UpDownFloor> m_currentFloor;//���ݏ���Ă��鏰
		float m_height;
		float m_radius;

		float m_moveSpeed;
		float m_maxSpeed;	//最高速
		float m_accel;
		Vec3 m_pos;
		Vec3 m_forward;
		Vec3 m_velocity;

		float m_ink;	//インク残量
		float m_inkMax;	//インク残量最大値
		float m_inkDecrease;	//インク減少量
		bool m_isDraw;	//インクが出せるかどうか

		Vec3 m_externalVelocity; // �O������̑��x
		
	public :
		// �X�e�[�W�������ɂ����R���X�g���N�^�y�K�{�z
		Player(const std::shared_ptr<Stage>& stage) :
			GameObject(stage), // �X�e�[�W��GameObject�ɓn���y�K�{�z
			m_height(1.0f),
			m_radius(0.49f),
			m_moveSpeed(0.0f),
			m_maxSpeed(4.0f),
			m_accel(0.99f),
			m_pos(Vec3(0.0f, 0.5f, 0.0f)),
			m_forward(Vec3(0)),
			m_velocity(Vec3(0)),
			m_ink(0.0f),
			m_inkMax(10.0f),
			m_inkDecrease(10.0f),
			m_isDraw(true),
			m_externalVelocity(Vec3(0))
		{
		}

		void OnCreate() override; // �����ݒ�p�̊֐�(Unity��Start���\�b�h�ɑ���)
		void OnUpdate() override; // ���t���[�����s�����֐�(Unity��Update���\�b�h�ɑ���)

		void OnMove();
		void DropInk();
		void OnDied();


		void OnCollisionEnter(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExit(std::shared_ptr<GameObject>& obj)override;

		void UpdateMoveFloor();

		// �O������̈ړ������Z����֐�
		//void AddExternalMove(const Vec3& move);
		//void SetExternalVelocity(const Vec3& v) { m_externalVelocity = v; }
	};

}
//end basecross

