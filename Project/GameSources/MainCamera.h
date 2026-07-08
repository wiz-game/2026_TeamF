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
	public:
		enum class CameraState
		{
			StartToGoal,	//ゴールに向かうカメラ演出
			TransitionToFollow,	//移行状態
			TargetFollow,	//ターゲット追従
		};

	private:
		CameraState cameraState = CameraState::StartToGoal;
		std::weak_ptr<GameObject>m_target;
		std::weak_ptr<GameObject> m_goal;		

		bool m_isPause = false;

		float m_cameraTimer = 0.0f;
		float m_stopTimer = 0.0f;

		float m_transitionTimer = 0.0f;
		Vec3 m_transitionStartEye;
		Vec3 m_transitionStartAt;

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

		void StartToGoalCameraMove(const Vec3& goalPos, float duration);
		void CameraMove();
		void TransitionToFollowMove(float time);

		void SetTarget(const std::weak_ptr<GameObject> target)
		{
			m_target = target;
		}

		//目的地設定
		void SetGoal(const std::weak_ptr<GameObject> goal)
		{
			m_goal = goal;
		}

		float GetAngleY() const
		{
			return m_angleY;
		}

		//カメラ演出ステートの設定
		void SetCameraState(CameraState state)
		{
			cameraState = state;
		}
		
		CameraState GetState()const
		{
			return cameraState;
		}
	};
}