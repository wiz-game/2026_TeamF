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
		m_transform->SetRotation(m_rot);

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetEmissive(Col4(0, 0, 0, 1));

		m_coll = AddComponent<CollisionObb>();
		m_coll->SetFixed(true);

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
		trans->SetPosition(Vec3(pos.x, pos.y + 0.1f, pos.z));
		trans->SetRotation(Vec3(m_rot.x, m_rot.y, m_rot.z + XM_PIDIV2));
	}

	void BeltConveyor::OnUpdate()
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

		//通電していれば床が動く
		if (isConnect)
		{
			m_isMove = true;

			//ベルトコンベアの回転から、移動方向を算出する
			float dx = sinf(m_rot.y);
			float dz = cosf(m_rot.y);
			Vec3 direction(dx, 0, dz);

			Vec3 movePos = direction * m_speed * delta; // 移動量を計算する

			m_floorDec->SetCurrentMoveVec(movePos);
			m_texObj->SetUVOffsetSpeed(Vec2(0, -m_speed * delta));
		}
		else
		{
			m_isMove = false;
			m_floorDec->SetCurrentMoveVec(Vec3(0));
			m_texObj->SetUVOffsetSpeed(Vec2(0, 0));

		}
	}
}
//end basecross
