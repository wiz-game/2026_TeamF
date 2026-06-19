/*!
@file GoalDoor.cpp
@brief ゴールの扉
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross
{
	void TrapDoor::OnCreate()
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

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetParent(m_owner);
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);

		
		//Drawコンポーネント
		m_draw = AddComponent<Texture2DrawComp>();
		m_draw->CreateTexture(m_scale.x, m_scale.z);
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetTextureResource(L"MoveFloor");
		m_draw->SetTexture2(L"Black");
		//m_draw->SetUvYInverrt(true);
		m_draw->SetDiffuse(Col4(1, 1, 1, 1));

		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void TrapDoor::OnUpdate()
	{
		if (m_isMove)
		{
			m_draw->SetDiffuse(Col4(1, 1, 0, 1));

		}
		else
		{
			m_draw->SetDiffuse(Col4(1, 1, 1, 1));

		}
	}

	//初期化
	void OffsetObject::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetScale(m_scale);
		m_transform->SetParent(m_owner);
		m_transform->SetPosition(0, 0.1f, 0);
	}

	void OffsetObject::OnUpdate()
	{

	}

}