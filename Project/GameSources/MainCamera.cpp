/*!
@file Player.cpp
@brief メインカメラ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	void MainCamera::OnCreate()
	{

	}

	void MainCamera::OnUpdate()
	{
		CameraMove();
	}

	void MainCamera::CameraMove()
	{
		auto& app = App::GetApp();
		auto delta = App::GetApp()->GetElapsedTime();

		//ターゲット(プレイヤー)
		auto target = m_target.lock();
		if (!target)
		{
			return;
		}

		//ターゲットの移動用コンポーネント取得
		auto targetTrans = target->GetComponent<Transform>();
		Vec3 targetPos = targetTrans->GetPosition();

		//位置
		Vec3 at = targetPos + Vec3(0.0f, 5.0f, 0.0f);
		//Vec3 at = targetPos + Vec3(0.0f, 1.0f, 0.0f);
		SetAt(at);

		//コントローラー取得
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		auto key = device.GetKeyState();

		Vec3 stickL(pad.fThumbLX, 0.0f, pad.fThumbLY);
		Vec3 stickR(pad.fThumbRX, 0.0f, pad.fThumbRY);
		//m_angleX -= pad.fThumbRY * delta;
		//m_angleY += -pad.fThumbLX * delta * GetAspect();

		float groundDis = 4.0f * cosf(m_angleX);
		Vec3 eye = Vec3(0);

		if (key.m_bPushKeyTbl['A'])
		{
			m_angleY += XMConvertToRadians(60.0f) * 1 * delta;
		}
		if (key.m_bPushKeyTbl['D'])
		{
			m_angleY -= XMConvertToRadians(60.0f) * 1 * delta;
		}

		//float length = stickR.length();
		//if (length != 0)
		//{
		//	m_angleY -= XMConvertToRadians(60.0f) * pad.fThumbRX * delta;
		//}
		//else
		//{
		//	m_angleY -= XMConvertToRadians(60.0f) * pad.fThumbRX * delta;
		//}

		m_angleY = XMConvertToRadians(60.0f) * 90.0f;
		eye = targetPos + Vec3(cos(m_angleY) * m_eyePos[0], m_eyePos[1], sinf(m_angleY) * 5.0f);
		//eye = targetPos + Vec3(cos(m_angleY), m_eyePos[1], sinf(m_angleY) * 5.0f);
		SetEye(eye);

	}
}