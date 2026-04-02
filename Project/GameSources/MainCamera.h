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

		float m_eyePos[3] = { 0.0f,2.0f,10.0f };
		float m_angleX = XMConvertToRadians(45.0f);
		float m_angleY = -XM_PIDIV2;
	public:
		void OnCreate();
		void OnUpdate();

		void CameraMove();
		float GetAngleY() const
		{
			return m_angleY;
		}

		void SetTarget(const std::weak_ptr<GameObject> target)
		{
			m_target = target;
		}
	};
}