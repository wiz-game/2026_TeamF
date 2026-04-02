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
		Vec3 at = targetPos;
		at.y += 1.0f;
		SetAt(at);

		//コントローラー取得
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];

		m_angleX -= pad.fThumbRY * delta;
		m_angleY -= pad.fThumbRX * delta * GetAspect();


		float groundDis = 4.0f * cosf(m_angleX);

		Vec3 dir;
		dir.x = groundDis * cosf(m_angleY);
		dir.y = 4.0f * sinf(m_angleX);
		dir.z = groundDis * sinf(m_angleY);

		Vec3 eye = targetPos + Vec3(cosf(m_angleY), m_eyePos[1], sinf(m_angleY) * m_eyePos[2]);

		dir.normalize();
		SetEye(at + dir * 6);

		//auto player = static_pointer_cast<Player>(target);
		//if (player)
		//{
		//	player->SetCameraAngleY();
		//}
	}
}