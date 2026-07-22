/*!
@file GoalDoor.cpp
@brief ゴールの扉
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	void GoalDoor::OnCreate()
	{
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"GoalDoor", path + L"GoalDoor.png"); // 画像ファイルを読み込んでアセットとして登録する
			app->RegisterTexture(L"Black", path + L"Black.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}

		//Drawコンポーネント
		m_draw = AddComponent<Texture2DrawComp>();
		m_draw->CreateTexture(m_scale.x, m_scale.z);
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetTextureResource(L"GoalDoor");
		m_draw->SetTexture2(L"Black");
		m_draw->SetBlendMode(BlendMode::Z_Axis);

		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);
		m_trans->SetRotation(m_rot);

		m_startPos = m_pos;
		
		Vec3 localMoveDir = Vec3(0.0f,0.0f,0.0f);
		if (m_side == DoorSide::Left)
		{
			localMoveDir = Vec3(-1.0f, 0.0f, 0.0f);//左扉はマイナス方向に移動
		}
		if (m_side == DoorSide::Right)
		{
			localMoveDir = Vec3(1.0f, 0.0f, 0.0f);//左扉はプラス方向に移動
		}

		//扉がどの向きを向いていても、その扉にとっての左右に動くようにする
		Quat rotQuat = m_trans->GetQuaternion();
		//ローカルベクトルの回転変換
		m_moveDir = rotQuat * localMoveDir;
		m_moveDir.normalize();

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);

		AddComponent<TextureCollision>();
	}

	void GoalDoor::OnUpdate()
	{

		if (!m_port) return;

		bool isConnect = m_port->GetConnect();
		if (isConnect)
		{
			m_isOpen = true;

			auto delta = App::GetApp()->GetElapsedTime();

			m_pos += m_moveDir * (m_speed * delta);

			float distance = (m_pos - m_startPos).length();
			
			if (distance >= m_scale.x)
			{
				m_pos = m_startPos + m_moveDir * m_scale.x;
				m_trans->SetPosition(m_pos);

				m_speed = 0;
			}
			m_trans->SetPosition(m_pos);
		}
	}
}