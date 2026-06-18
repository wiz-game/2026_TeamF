/*!
@file MoveFloor.cpp
@brief 上下する床実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void MoveFloor::OnCreate()
	{
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"MoveFloor", path + L"MoveFloor.png"); // 画像ファイルを読み込んでアセットとして登録する
			app->RegisterTexture(L"Black", path + L"Black.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}

		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);

		// ドローコンポーネントを追加
		m_staticDraw = AddComponent<Texture2DrawComp>();
		m_staticDraw->CreateTexture(m_scale.x,m_scale.z);
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		//m_staticDraw->SetEmissive(Col4(1, 1, 1, 1));
		m_staticDraw->SetDiffuse(Col4(1, 1, 1, 1));
		m_staticDraw->SetTextureResource(L"MoveFloor");
		m_staticDraw->SetTexture2(L"Black");
		m_staticDraw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		m_coll = AddComponent<CollisionObb>();
		m_coll->SetFixed(true);

		m_floorDec = GetStage()->AddGameObject<FloorDecision>(GetThis<MoveFloor>());

		try
		{	// objectの取得
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		}
		catch (...) {
		}

		AddComponent<TextureCollision>();
	}

	void MoveFloor::OnUpdate()
	{
		if (m_port)
		{
			isConnect = m_port->GetConnect();
		}
		else
		{
			isConnect = true;
		}

		auto scene = App::GetApp()->GetScene<Scene>();
		float delta = App::GetApp()->GetElapsedTime();
		Vec3 pos = m_transform->GetPosition();
		Vec3 oldPos = pos;
		Vec3 newPos = pos; // 移動後の位置を計算するための変数

		//通電していれば床が動く
		if (isConnect)
		{
			if (!m_MoveSound) {
				m_MoveSound = SoundManager::Get().PlayLoopSE(L"ELEVATER_MOVE",0.1f);
			}
			m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
			m_staticDraw->SetEmissive(Col4(1, 1, 0, 1));

			if (m_stopTimer > 0.0f)
			{
				m_stopTimer -= delta;
				m_floorDec->SetCurrentMoveVec(Vec3(0, 0, 0)); // 停止中は移動量をゼロにする
				return;	//このフレームの更新はここまで
			}

			//移動前の位置を覚えておく
			m_isUp = true;

			switch (m_moveAxis)
			{
			case MoveAxis::X:
				newPos.x += m_speed * delta;
				//limitDistが正の値なら正方向に、負の値なら負方向に動くようにする
				if (m_limitDist > 0.0f)
				{
					if (newPos.x > m_pos.x + m_limitDist)
					{
						newPos.x = m_pos.x + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
					else if (newPos.x < m_pos.x)
					{
						newPos.x = m_pos.x; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
				}
				else
				{
					if (newPos.x < m_pos.x + m_limitDist)
					{
						newPos.x = m_pos.x + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
					else if (newPos.x > m_pos.x)
					{
						newPos.x = m_pos.x; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
				}
				break;

			case MoveAxis::Y:
				newPos.y += m_speed * delta;
				//limitDistが正の値なら正方向に、負の値なら負方向に動くようにする
				if (m_limitDist > 0.0f)
				{
					if (newPos.y > m_pos.y + m_limitDist)
					{
						newPos.y = m_pos.y + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
					else if (newPos.y < m_pos.y)
					{
						newPos.y = m_pos.y; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
				}
				else
				{
					if (newPos.y < m_pos.y + m_limitDist)
					{
						newPos.y = m_pos.y + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
					else if (newPos.y > m_pos.y)
					{
						newPos.y = m_pos.y; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
				}
				break;

			case MoveAxis::Z:
				newPos.z += m_speed * delta;
				//limitDistが正の値なら正方向に、負の値なら負方向に動くようにする
				if (m_limitDist > 0.0f)
				{
					if (newPos.z > m_pos.z + m_limitDist)
					{
						newPos.z = m_pos.z + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
					else if (newPos.z < m_pos.z)
					{
						newPos.z = m_pos.z; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
				}
				else
				{
					if (newPos.z < m_pos.z + m_limitDist)
					{
						newPos.z = m_pos.z + m_limitDist; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
					else if (newPos.z > m_pos.z)
					{
						newPos.z = m_pos.z; // 強制的に限界地へ
						m_speed *= -1.0f;
						m_stopTimer = PAUSE_TIME;
					}
				}
				break;
			}
			m_transform->SetPosition(newPos);

			Vec3 moveDelta = newPos - oldPos;
			//計算した移動量をFloorDecisionに渡す
			m_floorDec->SetCurrentMoveVec(moveDelta);
		}
		else
		{
			m_staticDraw->SetDiffuse(Col4(1, 1, 1, 1));
			m_staticDraw->SetEmissive(Col4(1, 1, 1, 1));

			m_isUp = false;
			//床を元の位置に戻すための速度を計算する
			float returnSpeed = fabsf(m_speed) * 0.5f;
			float diff = 0.0f; // 現在の位置と元の位置の差

			switch (m_moveAxis)
			{
			case MoveAxis::X:
				diff = newPos.x - m_pos.x;
				if (fabs(diff) > 0.001f) // ある程度の差がある場合のみ移動する
				{
					if (diff > 0)//右にいるなら左に戻る
					{
						newPos.x -= returnSpeed * delta;
						if (newPos.x < m_pos.x) newPos.x = m_pos.x;
					}
					else//左にいるなら右に戻る
					{
						newPos.x += returnSpeed * delta;
						if (newPos.x > m_pos.x) newPos.x = m_pos.x;
					}
				}
				break;
			case MoveAxis::Y:
				diff = newPos.y - m_pos.y;
				if (fabs(diff) > 0.001f) // ある程度の差がある場合のみ移動する
				{
					if (diff > 0)//上にいるなら下に戻る
					{
						newPos.y -= returnSpeed * delta;
						if (newPos.y < m_pos.y) newPos.y = m_pos.y;
					}
					else//下にいるなら上に戻る
					{
						newPos.y += returnSpeed * delta;
						if (newPos.y > m_pos.y) newPos.y = m_pos.y;
					}
				}
				break;
			case MoveAxis::Z:
				diff = newPos.z - m_pos.z;
				if (fabs(diff) > 0.001f) // ある程度の差がある場合のみ移動する
				{
					if (diff > 0)//前にいるなら後ろに戻る
					{
						newPos.z -= returnSpeed * delta;
						if (newPos.z < m_pos.z) newPos.z = m_pos.z;
					}
					else//後ろにいるなら前に戻る
					{
						newPos.z += returnSpeed * delta;
						if (newPos.z > m_pos.z) newPos.z = m_pos.z;
					}
				}
				break;
			}

			m_speed = fabs(m_speed); // 速度を正の値にする
			m_transform->SetPosition(newPos);

			//Vec3 moveDelta = newPos - oldPos;
			//計算した移動量をFloorDecisionに渡す
			//m_floorDec->SetCurrentMoveVec(moveDelta);
			m_floorDec->SetCurrentMoveVec(Vec3(0, 0, 0)); // 通電していないときは移動量をゼロにする
		}
	}
	void MoveFloor::OnDestroy() {
		if (m_MoveSound) {
			SoundManager::Get().StopLoopSE(m_MoveSound);
		}
	}
}
//end basecross
