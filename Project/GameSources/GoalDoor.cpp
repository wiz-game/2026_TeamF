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

		Mat4x4 worldMat = m_trans->GetWorldMatrix();
		Vec3 localForward = Vec3(worldMat._31, worldMat._32, worldMat._33);
		localForward.normalize();

		if (m_side == DoorSide::Left)
		{
			m_moveDir = -localForward;//左扉はマイナス方向に移動
		}
		if (m_side == DoorSide::Right)
		{
			m_moveDir = localForward;//左扉はプラス方向に移動
		}

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

			if (distance >= m_scale.z)
			{
				m_pos = m_startPos + m_moveDir * m_scale.z;
				m_trans->SetPosition(m_pos);

				m_speed = 0;
			}
			m_trans->SetPosition(m_pos);
		}
	}
};