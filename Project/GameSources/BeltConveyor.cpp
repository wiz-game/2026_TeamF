/*!
@file BeltConveyor.cpp
@brief ベルトコンベアー
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void BeltConveyor::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		//m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetEmissive(Col4(0, 0, 1, 1));

		//m_coll = AddComponent<CollisionObb>();
		//m_coll->SetFixed(true);

		m_floorDec = GetStage()->AddGameObject<FloorDecision>(GetThis<BeltConveyor>());

		try
		{	// objectの取得
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");

			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"BeltConveyor", path + L"BeltConveyor.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}

		BeltConveyorTex::InitParams params;
		params.textureKey = L"BeltConveyor"; // テクスチャリソースのキー
		params.sides = 16; // 円形の分割数
		params.height = m_scale.x; // オーラの高さ
		params.radiusX = m_scale.y; // xの半径
		params.radiusZ = m_scale.z / 2.0f + 0.1f; // zの半径
		params.topColor = Col4(0.0f, 0.0f, 0.0f, 1.0f); // 上部の色
		params.bottomColor = Col4(0.0f, 0.0f, 0.0f, 1.0f); // 下部の色
		params.uvOffsetSpeed = Vec2(0.0f, 0.0f); // UVアニメーションの秒速
		params.textureLoops = 1.0f; // テクスチャの「u座標」

		m_texObj = GetStage()->AddGameObject<BeltConveyorTex>(params);

		auto trans = m_texObj->GetComponent<Transform>();
		auto pos = m_transform->GetPosition();
		trans->SetPosition(Vec3(pos.x + m_scale.x / 2.0f, pos.y + 0.1f, pos.z));
	}

	void BeltConveyor::OnUpdate()
	{
		if (!m_port) return; // ポートがいなければ何もしない

		auto scene = App::GetApp()->GetScene<Scene>();
		bool isConnect = m_port->GetConnect();
		float delta = App::GetApp()->GetElapsedTime();
		Vec3 pos = m_transform->GetPosition();
		Vec3 newPos = pos; // 移動後の位置を計算するための変数

		//通電していれば床が動く
		if (isConnect)
		{
			m_isMove = true;
			m_floorDec->SetCurrentMoveVec(Vec3(0, 0, m_speed * delta));

			m_texObj->SetUVOffsetSpeed(Vec2(0, -m_speed * delta));
		}
		else
		{
			m_isMove = false;
			m_floorDec->SetCurrentMoveVec(Vec3(0, 0, 0));
			m_texObj->SetUVOffsetSpeed(Vec2(0, 0));

		}
	}
}
//end basecross
