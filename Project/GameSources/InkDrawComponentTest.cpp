/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "InkDrawComponentTest.h"
namespace basecross{
	IMPLEMENT_DX11_VERTEX_SHADER(InkVertexShader, App::GetApp()->GetShadersPath() + L"InkVertexShader.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(InkPixelShader, App::GetApp()->GetShadersPath() + L"InkPixelShader.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(PNTPixelShader, App::GetApp()->GetShadersPath() + L"PSPNTInkDraw.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(CBBrushData)

	InkDrawComponentTest::InkDrawComponentTest(const shared_ptr<GameObject>& ptr, UINT sizeX, UINT sizeY):PNTStaticDraw(ptr){
		m_View = {};
		m_View.Width = sizeX;
		m_View.Height = sizeY;
		m_View.MinDepth = 0.0f;
		m_View.MaxDepth = 1.0f;

		CreateTexture(sizeX, sizeY);
	}
	InkDrawComponentTest::~InkDrawComponentTest(){}

	void InkDrawComponentTest::CreateTexture(UINT sizeX, UINT sizeY) {
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = sizeX;
		desc.Height = sizeY;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;

		// RTVとして使う + Shaderから読む
		desc.BindFlags =
			D3D11_BIND_RENDER_TARGET |
			D3D11_BIND_SHADER_RESOURCE;

		auto device = App::GetApp()->GetDeviceResources()->GetD3DDevice();
		auto deviceContext = App::GetApp()->GetDeviceResources()->GetD3DDeviceContext();
		auto result = device->CreateTexture2D(&desc, nullptr, m_Texture.GetAddressOf());
		assert(SUCCEEDED(result));
		result = device->CreateRenderTargetView(m_Texture.Get(), nullptr, m_RenderTargetView.GetAddressOf());
		assert(SUCCEEDED(result));
		result = device->CreateShaderResourceView(m_Texture.Get(), nullptr, m_ShaderResourceView.GetAddressOf());
		assert(SUCCEEDED(result));

		float clearColor[4] = { 0,0,0,0 };

		deviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	}

	void InkDrawComponentTest::AddDrawPoint(const Vec2& points) {
		m_DrawPoints.push_back(Vec4(points.x, points.y, 0.0f, 0.0f));
	}
	void InkDrawComponentTest::DrawInk() {
		if (m_DrawPoints.size() <= 0) return;
		MeshPrimData data = App::GetApp()->GetResource<MeshResource>(L"DEFAULT_PT_SQUARE_2")->GetMashData();
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
		pD3D11DeviceContext->VSSetShader(InkVertexShader::GetPtr()->GetShader(), nullptr, 0);
		//インプットレイアウトの設定
		pD3D11DeviceContext->IASetInputLayout(InkVertexShader::GetPtr()->GetInputLayout());
		//ピクセルシェーダ
		pD3D11DeviceContext->PSSetShader(InkPixelShader::GetPtr()->GetShader(), nullptr, 0);
		//個別処理
		for (int i = 0; i < m_DrawPoints.size(); i++) {
			m_Brush.m_Centers[i] = m_DrawPoints[i];
		}
		m_Brush.m_CenterCount = m_DrawPoints.size();

		//コンスタントバッファの更新
		pD3D11DeviceContext->UpdateSubresource(CBBrushData::GetPtr()->GetBuffer(), 0, nullptr, &m_Brush, 0, 0);
		//コンスタントバッファの設定
		ID3D11Buffer* pConstantBuffer = CBBrushData::GetPtr()->GetBuffer();
		ID3D11Buffer* pNullConstantBuffer = nullptr;
		//ピクセルシェーダに渡す
		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
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
		//サンプラーもクリア
		RenderState->SetSamplerAllClear(pD3D11DeviceContext);
		//ラスタライザステート(裏描画)
		pD3D11DeviceContext->RSSetState(RenderState->GetCullNone());
		pD3D11DeviceContext->DrawIndexed(data.m_NumIndicis, 0, 0);

		TextureMeshManager::Get().AddReload(GetGameObject()->GetComponent<TextureCollision>());
		m_DrawPoints.clear();
	}
	void InkDrawComponentTest::OnDraw() {
		auto deviceRes = App::GetApp()->GetDeviceResources();
		auto deviceContext = deviceRes->GetD3DDeviceContext();

		deviceContext->RSSetViewports(1, &m_View);
		deviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);

		DrawInk();
		
		deviceRes->StartDefaultDraw();

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
				DrawStatic<VSPNTStaticShadow>(PtrMeshResource->GetMashData());
			}
			else {
				DrawStatic<VSPNTStatic>(PtrMeshResource->GetMashData());
			}
		}
	}
}
//end basecross
