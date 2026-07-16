#include "stdafx.h"
#include "InkDrawComp.h"
#include "game_controller.h"
#include "Player.h"
#include "TextureCollision.h"
#include "GameProgressManager.h"
namespace basecross
{
	void InkDrawComp::OnCreate()
	{
		App::GetApp()->RegisterTexture(L"InkTest", App::GetApp()->GetDataDirWString() + L"Texture/Test/InkCollisionTest3.png");
		m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		
	}

	void InkDrawComp::OnUpdate()
	{
		if (!m_player && !isInkDrow) return;
		auto pTrans = m_player->GetComponent<Transform>();
		auto size = m_defaultSize * 0.5f;
		AddPointFromWorldPos(pTrans->GetWorldPosition());
		SetBrushSize(size);

		if (GameController::IsPressed_ButtonRight() && GameController::IsPressed_DpadLeft()) {
			int stageNum = GameProgressManager::Get().GetCurrentStage();
			wstring filename = L"DefaultInk_Stage" + to_wstring(stageNum) + L"_";
			auto object = GetGameObject();
			auto objBase = dynamic_pointer_cast<StageObjBase>(object);

			int id = (int)(objBase->m_pos.x + objBase->m_pos.y + objBase->m_pos.z);
			filename += objBase->m_type + to_wstring(id);
			TextureWriter::Write(filename, m_texture.Get());
		}

	}

	void InkDrawComp::InkDrawStart()
	{
		if (!isInkDrow) return;
		auto dev = App::GetApp()->GetDeviceResources();
		auto devContext = dev->GetD3DDeviceContext();//描画するためのデバイスコンテキストの取得

		devContext->RSSetViewports(1, &m_viewport);//ビューポートの設定
		devContext->OMSetRenderTargets(1, m_textureRTV.GetAddressOf(), nullptr);//レンダーターゲットの設定
		InkDraw();//インクの描画
		dev->StartDefaultDraw();//描画前の準備

	}

	void InkDrawComp::OnDraw()
	{
		InkDrawStart();

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
				DrawStatic<InkDrawShadowVertexSheder, InkDrawShadowPixelSheder>(PtrMeshResource->GetMashData());
			}
			else {
				DrawStatic<InkDrawVertexSheder, InkDrawPixelSheder>(PtrMeshResource->GetMashData());
			}
		}
		//後始末
		auto Dev = App::GetApp()->GetDeviceResources();
		Dev->InitializeStates();

	}

	void InkDrawComp::CreateTexture(float scaleX, float scaleZ)
	{
		UINT width = (UINT)m_defaultSize * scaleX;
		UINT height = (UINT)m_defaultSize * scaleZ;

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = width;
		desc.Height = height;

		desc.MipLevels = 1;//ミップマップレベル数
		desc.ArraySize = 1;//テクスチャ配列のサイズ
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//テクスチャのフォーマット
		desc.SampleDesc.Count = 1;//マルチサンプリングのサンプル数
		desc.Usage = D3D11_USAGE_DEFAULT;//テクスチャの使用方法
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;//テクスチャのバインドフラグ

		auto device = App::GetApp()->GetDeviceResources()->GetD3DDevice();
		//色を塗る対象
		device->CreateTexture2D(&desc, nullptr, m_texture.GetAddressOf());
		//読込
		device->CreateShaderResourceView(m_texture.Get(), nullptr, m_textureSRV.GetAddressOf());//シェーダーリソースビューの作成
		//書き込み
		device->CreateRenderTargetView(m_texture.Get(), nullptr, m_textureRTV.GetAddressOf());//レンダーターゲットビューの作成

		 D3D11_VIEWPORT viewport{};
		 viewport.Width = static_cast<FLOAT>(width);
		 viewport.Height = static_cast<FLOAT>(height);
		 viewport.MinDepth = 0.0f;//表示するｚ軸の最小の幅
		 viewport.MaxDepth = 1.0f;//表示するｚ軸の最大
		 m_viewport = viewport;

		 auto dev = App::GetApp()->GetDeviceResources();
		 auto devContext = dev->GetD3DDeviceContext();//描画するためのデバイスコンテキストの取得

		 float clearColor[4] = { 0,0,0,0 };//クリアカラーの設定
		 devContext->ClearRenderTargetView(m_textureRTV.Get(), clearColor);//全ピクセルの初期化
		 m_brush.textrueWidth = width;
		 m_brush.textrueHeight = height;
	}

	void InkDrawComp::CreateTexture(const wstring& texKey)
	{
		auto texResource = App::GetApp()->GetResource<TextureResource>(texKey);

		auto texSRV = texResource->GetShaderResourceView();

		ID3D11Resource* tempResource = nullptr;
		ID3D11Texture2D* texture = nullptr;

		texSRV->GetResource(&tempResource);
		tempResource->QueryInterface(__uuidof(ID3D11Texture2D),(void**)&texture);

		if (!texture) return;
		D3D11_TEXTURE2D_DESC desc{};
		texture->GetDesc(&desc);

		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;//テクスチャのバインドフラグ

		auto device = App::GetApp()->GetDeviceResources()->GetD3DDevice();
		//色を塗る対象
		device->CreateTexture2D(&desc, nullptr, m_texture.GetAddressOf());
		auto deviceContext = App::GetApp()->GetDeviceResources()->GetD3DDeviceContext();
		deviceContext->CopyResource(m_texture.Get(), texture);
		//読込
		device->CreateShaderResourceView(m_texture.Get(), nullptr, m_textureSRV.GetAddressOf());//シェーダーリソースビューの作成
		//書き込み
		device->CreateRenderTargetView(m_texture.Get(), nullptr, m_textureRTV.GetAddressOf());//レンダーターゲットビューの作成

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<FLOAT>(desc.Width);
		viewport.Height = static_cast<FLOAT>(desc.Height);
		viewport.MinDepth = 0.0f;//表示するｚ軸の最小の幅
		viewport.MaxDepth = 1.0f;//表示するｚ軸の最大
		m_viewport = viewport;

		auto dev = App::GetApp()->GetDeviceResources();
		auto devContext = dev->GetD3DDeviceContext();//描画するためのデバイスコンテキストの取得

		m_brush.textrueWidth = desc.Width;
		m_brush.textrueHeight = desc.Height;
	}


	void InkDrawComp::InkDraw()
	{
		if (m_brush.count <= 0) return;
		auto meshResource = App::GetApp()->GetResource<MeshResource>(L"DEFAULT_PT_SQUARE_2");
		MeshPrimData data = meshResource->GetMashData();
		auto Dev = App::GetApp()->GetDeviceResources();
		auto pD3D11DeviceContext = Dev->GetD3DDeviceContext();
		auto RenderState = Dev->GetRenderState();
		//NULLのシェーダリソースの準備
		ID3D11ShaderResourceView* pNull[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
		//サンプラーの準備
		ID3D11SamplerState* pNullSR[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
		//各オブジェクト共通処理
		//シェーダの設定
		//頂点シェーダ
		pD3D11DeviceContext->VSSetShader(InkDropVertexSheder::GetPtr()->GetShader(), nullptr, 0);
		//インプットレイアウトの設定
		pD3D11DeviceContext->IASetInputLayout(InkDropVertexSheder::GetPtr()->GetInputLayout());
		//ピクセルシェーダ
		pD3D11DeviceContext->PSSetShader(InkDropPixelSheder::GetPtr()->GetShader(), nullptr, 0);

		//インク用コンスタントバッファ
		pD3D11DeviceContext->UpdateSubresource(CBBrush::GetPtr()->GetBuffer(), 0, nullptr, &m_brush, 0, 0);
		ID3D11Buffer* InkResourceBuffer = CBBrush::GetPtr()->GetBuffer();
		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &InkResourceBuffer);

		//ストライドとオフセット
		UINT stride = data.m_NumStride;
		UINT offset = 0;
		//描画方法のセット
		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//頂点バッファのセット
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, data.m_VertexBuffer.GetAddressOf(), &stride, &offset);
		//インデックスバッファのセット
		pD3D11DeviceContext->IASetIndexBuffer(data.m_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		//各レンダリングステートの設定
		//ブレンドステート
		RenderState->SetBlendState(pD3D11DeviceContext, GetBlendState());
		//デプスステンシルステート
		RenderState->SetDepthStencilState(pD3D11DeviceContext, GetDepthStencilState());
		//シェーダーリソースもクリア
		pD3D11DeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pNull);
		//サンプラーを設定
		RenderState->SetSamplerState(pD3D11DeviceContext, GetSamplerState(), 0);
		//ラスタライザステート（表描画）
		pD3D11DeviceContext->RSSetState(RenderState->GetCullBack());
		//描画
		pD3D11DeviceContext->DrawIndexed(data.m_NumIndicis, 0, 0);

		TextureMeshManager::Get().AddReload(GetGameObject()->GetComponent<TextureCollision>());
		ClearPoint();

	}

	void InkDrawComp::AddPoint(const Vec3& point)
	{
		if (m_brush.count > 4)
		{
			return;
		}
		
		m_brush.centerPositions[m_brush.count] = Vec4(point.x,point.y,point.z,0);
		m_brush.count += 1;
	}

	void InkDrawComp::ClearPoint()
	{
		m_brush.count = 0;
	}

	void InkDrawComp::AddPointFromWorldPos(const Vec3& playerWorldPos)
	{
		//自分自身のトランスフォームを取得
		auto trans = GetGameObject()->GetComponent<Transform>();
		if (!trans) return;

		//自分自身の位置とスケールを習得
		Vec3 myPos = trans->GetWorldPosition();
		Vec3 myScale = trans->GetScale();
		Vec3 playerLocalPosition = (Vec3)XMVector3Transform(playerWorldPos,inverse(trans->GetWorldMatrix()));

		float relativeY = playerWorldPos.y - myPos.y;
		//cubeメッシュのサイズを考慮して一律の範囲に収める
		float localX = playerLocalPosition.x;
		float localY = playerLocalPosition.y;
		float localZ = playerLocalPosition.z;

		//テクスチャの座標（0.0～1.0）に変換
		float uvX = 0.0f;
		float uvY = 0.0f;

		//オブジェクトの前方向を取得
		Vec3 forward = trans->GetForward();

		if (forward.z >= 0.0f)
		{
			uvY = 1.0f - (localZ + 0.5f);
			uvX = localX + 0.5f;
		}
		else
		{
			uvY = localZ + 0.5f;//trapDoor用
			uvX = 1.0f - (localX + 0.5f);
		}
		
		//プレイヤーが自分の上に乗っているかつ、
		// Playerと接触している場合のみインクを塗る
		if (uvX >= 0.0f && uvX <= 1.0f && uvY >= 0.0f && uvY <= 1.0f && abs(relativeY) <= 1.2f)
		{
			if (GameController::IsPressed_ButtonDown())
			{
				AddPoint(Vec3(uvX, uvY, 0.0f));
				m_player->DecreaseInk();
			}
			if (GameController::IsPressed_ButtonRight())
			{
				AddPoint(Vec3(uvX, uvY, 1.0f));
			}

		}
	}


	IMPLEMENT_DX11_PIXEL_SHADER(InkDrawPixelSheder, App::GetApp()->GetShadersPath() + L"PSInkDraw.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(InkDrawVertexSheder, App::GetApp()->GetShadersPath() + L"VSInkDraw.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(CBInk)
	IMPLEMENT_DX11_PIXEL_SHADER(InkDrawShadowPixelSheder, App::GetApp()->GetShadersPath() + L"PSInkDrawShadow.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(InkDrawShadowVertexSheder, App::GetApp()->GetShadersPath() + L"VSInkDrawShadow.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(InkDropPixelSheder, App::GetApp()->GetShadersPath() + L"PSInkDrop.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(InkDropVertexSheder, App::GetApp()->GetShadersPath() + L"VSInkDrop.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(CBBrush)

}