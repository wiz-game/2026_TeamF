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
		Vec3 at = targetPos + Vec3(0.0f, 1.0f, 0.0f);
		SetAt(at);

		//コントローラー取得
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];

		//m_angleX -= pad.fThumbRY * delta;
		//m_angleY += -pad.fThumbLX * delta * GetAspect();

		float groundDis = 4.0f * cosf(m_angleX);

		m_angleY -= XMConvertToRadians(60.0f) * pad.fThumbLX * delta;
		Vec3 eye = targetPos + Vec3(cosf(m_angleY) * 5.0f, m_eyePos[1], sinf(m_angleY) * 5.0f);

		SetEye(eye);
	}
}