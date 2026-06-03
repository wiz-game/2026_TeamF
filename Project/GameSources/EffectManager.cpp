/*!
@file EffectManager.cpp
@brief エフェクトなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "EffectManager.h"

namespace basecross {
	shared_ptr<EffectManager> EffectManager::g_Instance = nullptr;
	//--------------------------------------------------------------------------------------
	///	Effekseerエフェクトのエフェクト
	//--------------------------------------------------------------------------------------
	EffectManager::EffectManager(const shared_ptr<Stage>& stage) :
		MultiParticle(stage),
		m_renderer(nullptr),
		m_Manager(nullptr)
	{
	}
	EffectManager::~EffectManager() {
		// 先にエフェクト管理用インスタンスを破棄
		m_Manager.Reset();
		// 次に描画用インスタンスを破棄
		m_renderer.Reset();
	}

	void EffectManager::OnCreate() {
		CreateEffectInterface();

		auto& app = App::GetApp();
		SetAlphaActive(true);

		g_Instance = GetThis<EffectManager>();
	}

	void EffectManager::OnUpdate()
	{
		auto elps = App::GetApp()->GetElapsedTime();
		
		m_TotalTime += elps;

		// エフェクトの更新処理を行う
		m_Manager->Update();
		m_renderer->SetTime(elps);
	}

	void EffectManager::OnDraw()
	{
		auto& camera = GetStage()->GetView()->GetTargetCamera();
		SetViewProj(camera->GetViewMatrix(), camera->GetProjMatrix());
		// エフェクトの描画開始処理を行う。
		m_renderer->BeginRendering();
		// エフェクトの描画を行う。
		m_Manager->Draw();
		
		// エフェクトの描画終了処理を行う。
		m_renderer->EndRendering();
	}

	void EffectManager::OnDestroy()
	{}

	void EffectManager::SetEffectSpeed(Effekseer::Handle& handle, const float& speed)
	{
		m_Manager->SetSpeed(handle, speed);
	}

	void EffectManager::SetEffectPause( const bool& pause)
	{
		m_Manager->SetPausedToAllEffects(pause);
	}

	void EffectManager::Mat4x4ToMatrix44(const bsm::Mat4x4& src, Effekseer::Matrix44& dest)
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				dest.Values[i][j] = src(i, j);
			}
		}
	}

	void EffectManager::SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj)
	{
		Effekseer::Matrix44 v, p;
		Mat4x4ToMatrix44(view, v);
		Mat4x4ToMatrix44(proj, p);
		m_renderer->SetCameraMatrix(v);
		m_renderer->SetProjectionMatrix(p);
	}

	void EffectManager::PlayEffect(Effekseer::Handle& handle, const wstring& Key, const bsm::Vec3& Emitter, const float freme)
	{
		int32_t Freme = freme;
		m_Effect = GetEffectResource(Key);
		handle = m_Manager->Play(m_Effect, ::Effekseer::Vector3D(Emitter.x, Emitter.y, Emitter.z), Freme);
	}

	void EffectManager::CreateEffectInterface()
	{

		auto Dev = App::GetApp()->GetDeviceResources();
		auto pDx11Device = Dev->GetD3DDevice();
		auto pID3D11DeviceContext = Dev->GetD3DDeviceContext();
		// 描画用インスタンスの生成
		m_renderer = EffekseerRendererDX11::Renderer::Create(pDx11Device, pID3D11DeviceContext, 4000);
		// エフェクト管理用インスタンスの生成
		m_Manager = Effekseer::Manager::Create(4000);

		// 描画用インスタンスから描画機能を設定
		m_Manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
		m_Manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
		m_Manager->SetRingRenderer(m_renderer->CreateRingRenderer());
		m_Manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
		m_Manager->SetModelRenderer(m_renderer->CreateModelRenderer());

		// 描画用インスタンスからテクスチャの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		m_Manager->SetTextureLoader(m_renderer->CreateTextureLoader());
		m_Manager->SetModelLoader(m_renderer->CreateModelLoader());
		m_Manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());
		m_Manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
	}

	void EffectManager::RegisterResource(const wstring& Key, const  wstring& FileName)
	{
		try {
			// キーが空文字列の場合は不正な呼び出しとして例外をスロー
			if (Key == L"") {
				throw BaseException(
					L"キーが空", // エラーメッセージ
					L"if(Key == L¥"")", // エラー箇所
					L"Effect::RegisterResource()" // 関数名
				);
			}

			// Effekseerを使用してファイルからエフェクトデータを読み込む
			auto Effect = Effekseer::Effect::Create(m_Manager, (const char16_t*)FileName.c_str());

			//--- 重複チェック ---
			map<wstring, Effekseer::EffectRef>::iterator it;
			// 1. 既に読み込まれたエフェクトデータと同じものではないかチェック
			for (it = m_ResMap.begin(); it != m_ResMap.end(); it++) {
				// 同じエフェクトデータが見つかった場合
				if (it->second == Effect)
				{
					// キーも同じなら、既に登録済みのため正常終了（二重登録防止）
					if (it->first == Key)
					{
						return;
					}
					// 同じエフェクトデータが「異なるキー」で登録されようとしているため、例外をスロー
					wstring keyerr = L"同じエフェクトリソースが別のキー(" + it->first + L")で既に登録されています。キー: " + Key;
					throw BaseException(
						L"リソースの重複登録エラー",
						keyerr,
						L"Effect::RegisterResource()"
					);
				}
			}

			// 2. 指定されたキーが既に使われていないかチェック
			it = m_ResMap.find(Key);
			if (it != m_ResMap.end())
			{
				// 指定のキーが見つかった（キーが重複している）ため、例外をスロー
				wstring keyerr = L"同じエフェクトリソースが別のキー(" + it->first + L")で既に登録されています。キー: " + Key;
				throw BaseException(
					L"キーの重複エラー",
					keyerr,
					L"Effect::RegisterResource()"
				);
			}
			else {
				// チェックをすべてパスした場合、リソースマップに登録する
				m_ResMap[Key] = Effect;
			}
		}
		catch (...) {
			// この関数内で発生した例外を、そのまま呼び出し元に投げる
			throw;
		}
	}

	Effekseer::EffectRef EffectManager::GetEffectResource(const wstring& Key)
	{
		// キーが空文字列の場合は不正な呼び出しとして例外をスロー
		if (Key == L"") {
			throw BaseException(
				L"キーが空",
				L"if(Key == "")",
				L"EffectManager::GetEffectResource()" // NOTE: EffectManager::GetEffectResource() がより正確かもしれません
			);
		}

		// マップからキーを検索（const_iteratorを使用）
		map<wstring, Effekseer::EffectRef >::const_iterator  it;
		it = m_ResMap.find(Key);

		if (it != m_ResMap.end()) {
			// 指定のキーが見つかった場合、対応するエフェクトリソースを返す
			return  it->second;
		}
		else {
			// キーが見つからなかったため、例外をスロー
			wstring keyerr = L"指定されたキー(" + Key + L")のリソースが見つかりません。\n";
			throw BaseException(
				L"",
				keyerr,
				L"EffectManager::GetEffectResource()" // NOTE: EffectManager::GetEffectResource() がより正確かもしれません
			);
		}
	}

	void EffectManager::AddLocation(Effekseer::Handle& handle, const bsm::Vec3& Location) {
		if (handle != -1) {
			m_Manager->AddLocation(handle, ::Effekseer::Vector3D(Location.x, Location.y, Location.z));
		}
	}

	void EffectManager::SetRotation(Effekseer::Handle& handle, const bsm::Vec3& Location, const float angle)
	{
		m_Manager->SetRotation(handle, ::Effekseer::Vector3D(Location.x, Location.y, Location.z), angle);
	}

	void EffectManager::SetLocation(Effekseer::Handle& handle, const bsm::Vec3& Location) {
		m_Manager->SetLocation(handle, Location.x, Location.y, Location.z);
	}
	void EffectManager::SetScale(Effekseer::Handle& handle, const bsm::Vec3& Scale)
	{
		m_Manager->SetScale(handle, Scale.x, Scale.y, Scale.z);
	}

	void EffectManager::SetAllColor(Effekseer::Handle& handle, const bsm::Col4 Color)
	{
		auto color = Col4(Color) * 255;
		m_Manager->SetAllColor(handle, ::Effekseer::Color(color.x, color.y, color.z, color.w));
	}

	void EffectManager::StopEffect(Effekseer::Handle& handle) {
		if (handle != -1) {
			m_Manager->StopEffect(handle);
		}
	}

	void EffectManager::SetLayer(Effekseer::Handle& handle, int32_t layer)
	{
		m_Manager->SetLayer(handle, layer);
	}

}
//end basecross
