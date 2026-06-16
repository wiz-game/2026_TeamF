/*!
@file Player.h
@brief メインカメラ
*/

#pragma once
#include "stdafx.h"

namespace basecross
{
	class MainCamera :public Camera
	{
		std::weak_ptr<GameObject>m_target;

		//角度
		float m_eyePos[3] = { 5.0f, 25.0f, 10.0f };
		//float m_eyePos[3] = { 0.0f, 5.0f, 10.0f };
		float m_angleX = XMConvertToRadians(45.0f);
		float m_angleY;
	public:
		MainCamera():
			m_angleY(-(XM_PIDIV2))
		{
		}

		~MainCamera()
		{
		}

		void OnCreate();
		void OnUpdate();

		void CameraMove();

		void SetTarget(const std::weak_ptr<GameObject> target)
		{
			m_target = target;
		}

		float GetAngleY() const
		{
			return m_angleY;
		}
	};
}