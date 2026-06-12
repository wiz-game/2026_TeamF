/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "InkDrawComponentTest.h"
namespace basecross
{
	Floor::Floor
	(
		const std::shared_ptr<Stage>& stage,
		const Vec3& Scale,
		const Vec3& rot,
		const Vec3& Pos
	) :
		StageObjBase(stage, Scale, rot, Pos),
		m_pos(Pos),
		m_scale(Scale)
	{
	}

	void Floor::OnCreate()
	{
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"Floor", path + L"Floor.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}

		//Drawコンポーネント
		//m_draw = AddComponent<PNTStaticDraw>();
		m_draw = AddComponent<InkDrawComponentTest>((int)(64.0f * m_scale.x), (int)(64.0f * m_scale.z));
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetTextureResource(L"Floor");
		//m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);
		m_trans->SetRotation(m_rot);


		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void Floor::OnUpdate()
	{

	}
}