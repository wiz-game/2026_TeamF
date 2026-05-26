/*!
@file Port.cpp
@brief ポート実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "Port.h"

namespace basecross {
	// 初期設定
	void Port::OnCreate()
	{
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"Port", path + L"Port.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);

		// ドローコンポーネントを追加
		//m_draw = AddComponent<PNTDXModelDraw>();
		//m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<UpperDrawCom>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(1, 0, 0, 1));
		m_staticDraw->SetTextureResource(L"Port");
		m_staticDraw->SetBlendState(BlendState::AlphaBlend);
		
		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		AddComponent<Electrified>();
		InkConnectChecker::Get().AddPort(GetThis<Port>());
	}

	// 更新処理
	void Port::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		auto elec = GetComponent<Electrified>();
		//電流の更新
		elec->UpdateElectrified();
		auto power = elec->IsPowered();//電流が流れているかどうかを更新

		// 電流の状態に応じて接続状態と色を更新
		if (power)
		{
			isConnect = true;
			m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
		}
		else
		{
			isConnect = false;
			m_staticDraw->SetDiffuse(Col4(1, 0, 0, 1));
		}
	}

	void Port::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			//リストに追加
			elec->OnElectrifiedEnter(obj);
		}
	}

	void Port::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			elec->OnElectrifiedExit(obj);
		}
	}

	void UpperDrawCom::OnDraw()
	{
		if (GetGameObject()->GetAlphaActive()) {
			if (!(GetBlendState() == BlendState::AlphaBlend || GetBlendState() == BlendState::Additive)) {
				SetBlendState(BlendState::AlphaBlend);
			}
			SetRasterizerState(RasterizerState::DoubleDraw);
		}
		//メッシュリソースの取得
		auto PtrMeshResource = GetMeshResource();
		if (PtrMeshResource) {
			if (GetOwnShadowActive()) {
				DrawStatic<VSPNTStaticShadow, UpperShadowPixelSheder>(PtrMeshResource->GetMashData());
			}
			else {
				DrawStatic<VSPNTStatic, UpperPixelSheder>(PtrMeshResource->GetMashData());
			}
		}
		//マルチメッシュリソースの取得
		auto PtrMultiMeshResource = GetMultiMeshResource();
		if (PtrMultiMeshResource) {
			size_t count = PtrMultiMeshResource->GetMeshVecCount();
			auto& vec = PtrMultiMeshResource->GetMeshVec();
			for (size_t i = 0; i < count; i++) {
				if (GetMultiMeshIsDraw(i)) {
					if (GetOwnShadowActive()) {
						if (GetGameObject()->GetComponent<Shadowmap>(false)) {
							DrawStatic<VSPNTStaticShadow, PSPNTStaticShadow2>(vec[i]);
						}
						else {
							DrawStatic<VSPNTStaticShadow, UpperShadowPixelSheder>(vec[i]);
						}
					}
					else {
						DrawStatic<VSPNTStatic, UpperPixelSheder>(vec[i]);
					}
				}
			}
		}
		//後始末
		auto Dev = App::GetApp()->GetDeviceResources();
		Dev->InitializeStates();

	}

	IMPLEMENT_DX11_PIXEL_SHADER(UpperPixelSheder, App::GetApp()->GetShadersPath() + L"PSUpper.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(UpperShadowPixelSheder, App::GetApp()->GetShadersPath() + L"PSUpperShadow.cso")

}
//end basecross

