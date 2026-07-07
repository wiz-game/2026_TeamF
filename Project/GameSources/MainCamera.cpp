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
		auto goal = m_goal.lock();
		if (!goal)
		{
			//ゴールがなかったらカメラ演出無し
			cameraState = CameraState::TargetFollow; 
			CameraMove();
			return;
		}

		Vec3 goalPos = goal->GetComponent<Transform>()->GetWorldPosition();

		switch (cameraState)
		{
		case CameraState::StartToGoal:
			// ゴールに向かうカメラ演出
			StartToGoalCameraMove(goalPos, 4.0f);
			break;
		case CameraState::TransitionToFollow:
			//カメラ演出からターゲット追従にスムーズに移行する
			TransitionToFollowMove(0.5f);
			break;
		case CameraState::TargetFollow:
			// ターゲット追従
			CameraMove();
			break;
		}
	}

	//ゴールに向かうカメラ演出
	void MainCamera::StartToGoalCameraMove(const Vec3& goalPos, float duration)
	{
		auto target = m_target.lock();
		if (!target) return;

		Vec3 pPos = target->GetComponent<Transform>()->GetWorldPosition();
		float delta = App::GetApp()->GetElapsedTime();
		float stopDuration = 0.2f;//ゴール前で止まる時間

		//タイマーを更新
		m_cameraTimer += delta;
		//片道の時間を計算
		float halfMoveDuration = (duration - stopDuration) * 0.5f;
		
		float phase1_End = halfMoveDuration;				//住路終了タイミング
		float phase2_End = halfMoveDuration + stopDuration;	//停止終了タイミング

		m_angleY = XMConvertToRadians(60.0f) * 90.0f;
		//Vec3 cameraOffset = Vec3(cos(m_angleY) * m_eyePos[0], m_eyePos[1], sinf(m_angleY) * 5.0f);
		Vec3 cameraOffset = Vec3(m_eyePos[0], m_eyePos[1], sinf(m_angleY) * 5.0f);

		Vec3 startAt = Vec3(pPos.x, pPos.y + 5.0f, pPos.z);
		Vec3 endAt = Vec3(goalPos.x, goalPos.y + 5.0f, goalPos.z);
		Vec3 currentAt;

		if (m_cameraTimer < phase1_End)
		{
			//プレイヤーからゴールへ
			float t = m_cameraTimer / halfMoveDuration;

			//SmoothStepでイージングをかける（滑らかな加速度）
			float smoothT = t * t * (3.0f - 2.0f * t);
			currentAt = startAt + (endAt - startAt) * smoothT;
		}
		else if (m_cameraTimer < phase2_End)
		{
			//しばらく止まる
			currentAt = endAt;
		}
		else if (m_cameraTimer < duration)
		{
			//戻る
			float backTime = m_cameraTimer - phase2_End;//復路が始まってからの経過時間の計算
			float t = backTime / halfMoveDuration;
			
			float smoothT = t * t * (3.0f - 2.0f * t);

			currentAt = endAt + (startAt - endAt) * smoothT;
		}
		else
		{
			m_cameraTimer = 0.0f;

			m_transitionTimer = 0.0f;
			m_transitionStartAt = GetAt();
			m_transitionStartEye = GetEye();

			//ステートを移行状態に変更
			cameraState = CameraState::TransitionToFollow;
			return;
		}

		SetAt(currentAt);
		SetEye(currentAt + cameraOffset);
	}

	//追従位置へのスムーズな移行
	void MainCamera::TransitionToFollowMove(float time)
	{
		auto target = m_target.lock();
		if (!target) return;

		float delta = App::GetApp()->GetElapsedTime();
		m_transitionTimer += delta;

		float t =  m_transitionTimer / time;
		if (t > 1.0f) t = 1.0f;

		float smoothT = t * t * (3.0f - 2.0f * t);

		Vec3 targetPos = target->GetComponent<Transform>()->GetWorldPosition();

		// 位置
		Vec3 idealAt = targetPos + Vec3(0.0f, 5.0f, 0.0f);

		// Eyeの位置
		m_angleY = XMConvertToRadians(60.0f) * 90.0f;
		Vec3 idealEye = targetPos + Vec3(cos(m_angleY) * m_eyePos[0], m_eyePos[1], sinf(m_angleY) * 5.0f);

		// 演出終了位置 から 理想の位置 への線形補間（Lerp）
		Vec3 currentEye = m_transitionStartEye + (idealEye - m_transitionStartEye) * smoothT;
		Vec3 currentAt = m_transitionStartAt + (idealAt - m_transitionStartAt) * smoothT;

		SetEye(currentEye);
		SetAt(currentAt);

		// 移行完了
		if (t >= 1.0f)
		{
			cameraState = CameraState::TargetFollow;
		}

	}

	//ターゲット追従
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
		Vec3 targetPos = targetTrans->GetWorldPosition();

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