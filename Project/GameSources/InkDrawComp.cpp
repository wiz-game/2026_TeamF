#include "stdafx.h"
#include "InkDrawComp.h"

namespace basecross
{
	void InkDrawComp::OnCreate()
	{
		App::GetApp()->RegisterTexture(L"InkTest", App::GetApp()->GetDataDirWString() + L"Texture/Test/InkCollisionTest3.png");
	}

	void InkDrawComp::OnUpdate()
	{
	}

	void InkDrawComp::OnDraw()
	{
		auto dev = App::GetApp()->GetDeviceResources();
		auto devContext = dev->GetD3DDeviceContext();//描画するためのデバイスコンテキストの取得

		devContext->RSSetViewports(1, &m_viewport);//ビューポートの設定
		devContext->OMSetRenderTargets(1, m_textureRTV.GetAddressOf(), nullptr);//レンダーターゲットの設定
		InkDraw();//インクの描画
		dev->StartDefaultDraw();//描画前の準備



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

	void InkDrawComp::CreateTexture(UINT width, UINT height)
	{
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
		 viewport.TopLeftX = 0;
		 viewport.TopLeftY = 0;
		 viewport.Width = static_cast<FLOAT>(width);
		 viewport.Height = static_cast<FLOAT>(height);
		 viewport.MinDepth = 0.0f;//表示するｚ軸の最小の幅
		 viewport.MaxDepth = 1.0f;//表示するｚ軸の最大
		 m_viewport = viewport;

		 auto dev = App::GetApp()->GetDeviceResources();
		 auto devContext = dev->GetD3DDeviceContext();//描画するためのデバイスコンテキストの取得

		 float clearColor[4] = { 0,0,0,0 };//クリアカラーの設定
		 devContext->ClearRenderTargetView(m_textureRTV.Get(), clearColor);//全ピクセルの初期化
	}

	void InkDrawComp::InkDraw()
	{

	}

	IMPLEMENT_DX11_PIXEL_SHADER(InkDrawPixelSheder, App::GetApp()->GetShadersPath() + L"PSInkDraw.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(InkDrawVertexSheder, App::GetApp()->GetShadersPath() + L"VSInkDraw.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(CBInk)
	IMPLEMENT_DX11_PIXEL_SHADER(InkDrawShadowPixelSheder, App::GetApp()->GetShadersPath() + L"PSInkDrawShadow.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(InkDrawShadowVertexSheder, App::GetApp()->GetShadersPath() + L"VSInkDrawShadow.cso")

}