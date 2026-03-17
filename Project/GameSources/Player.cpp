/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"

namespace basecross{
	// プレイヤーの初期設定
	void Player::OnCreate()
	{
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(Vec3(0.0f, 0.0f, 0.0f));
		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetDiffuse(Col4(1, 1, 1, 1));

		auto draw = AddComponent<PNTStaticDraw>();
		draw->SetMeshResource(L"DEFAULT_CUBE");
	}

	// プレイヤーの更新処理
	void Player::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		// 前回からの経過時間：デルタタイムを取得する
		float delta = app->GetElapsedTime();

		auto sensor = GameController::GetSensorData();
		Vec3 gyro = Vec3(sensor.gyroX, sensor.gyroY, sensor.gyroZ);

		auto quat = m_transform->GetQuaternion();
		//auto rotZ = (Quat)XMQuaternionRotationAxis(Vec3(1, 0, 0), gyro.z);
		auto rotY = (Quat)XMQuaternionRotationAxis(Vec3(0, 1, 0), gyro.y);
		//auto rotX = (Quat)XMQuaternionRotationAxis(Vec3(0, 0, 1), gyro.x);

		//quat = quat * rotX * rotY * rotZ;
		quat = quat * rotY;
		m_transform->SetQuaternion(rotY);
	}
}
//end basecross

