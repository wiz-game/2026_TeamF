/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	struct BrushData {
		Vec4 m_Centers[4];
		float m_Size;
		int m_CenterCount;
		float padding[2];
	};
	struct TimeBuffer {
		float m_Time = 0.0f;
		int padding[3];
	};
	class InkDrawComponentTest : public PNTStaticDraw {
		ComPtr<ID3D11Texture2D> m_Texture;
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;

		ComPtr<ID3D11ShaderResourceView> m_NormalMap;

		D3D11_VIEWPORT m_View;

		vector<Vec4> m_DrawPoints;
		BrushData m_Brush;
		TimeBuffer m_TimeBuffer;


		void CreateTexture2D(UINT sizeX, UINT sizeY);
		void CreateTexture2D(D3D11_TEXTURE2D_DESC desc);
		void CreateResource();

		void CreateTexture(UINT sizeX, UINT sizeY);
		void CreateTexture(const wstring& tex);

		void DrawInk();
	public:
		InkDrawComponentTest(const shared_ptr<GameObject>& ptr, UINT sizeX, UINT sizeY,const wstring& tex = L"");
		~InkDrawComponentTest();

		virtual void OnUpdate()override;
		virtual void OnDraw()override;

		void AddDrawPoint(const Vec2& points,bool eraser = false);
		void SetBrushSize(float size) {
			m_Brush.m_Size = size;
		}
		ComPtr<ID3D11ShaderResourceView> GetInkShaderResourceView()const {
			return m_ShaderResourceView;
		}

		void SetNormalMap(const wstring& texKey);

		template<typename T_VShader>
		void DrawStatic(const MeshPrimData& data) {
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
			pD3D11DeviceContext->VSSetShader(T_VShader::GetPtr()->GetShader(), nullptr, 0);
			//インプットレイアウトの設定
			pD3D11DeviceContext->IASetInputLayout(T_VShader::GetPtr()->GetInputLayout());
			//ピクセルシェーダ
			pD3D11DeviceContext->PSSetShader(PNTInkPixelShader::GetPtr()->GetShader(), nullptr, 0);
			//個別処理
			SimpleConstants SmCb;
			//コンスタントバッファの作成
			SetConstants(SmCb, data);
			//テクスチャ
			auto shTex = GetTextureResource();
			if (shTex) {
				//テクスチャがある
				SmCb.ActiveFlg.x = 1;
			}
			else {
				//描画コンポーネントにはテクスチャがない
				if (shTex = data.m_TextureResource.lock()) {
					//テクスチャがある
					SmCb.ActiveFlg.x = 1;
				}
				else {
					SmCb.ActiveFlg.x = 0;
				}
			}
			//コンスタントバッファの更新
			pD3D11DeviceContext->UpdateSubresource(CBSimple::GetPtr()->GetBuffer(), 0, nullptr, &SmCb, 0, 0);
			//コンスタントバッファの設定
			ID3D11Buffer* pConstantBuffer = CBSimple::GetPtr()->GetBuffer();
			ID3D11Buffer* pNullConstantBuffer = nullptr;
			//頂点シェーダに渡す
			pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
			//ピクセルシェーダに渡す
			//pD3D11DeviceContext->PSSetConstantBuffers(0, 2, psConstantBuffers);
			pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
			//ストライドとオフセット
			UINT stride = data.m_NumStride;
			UINT offset = 0;
			//描画方法のセット
			pD3D11DeviceContext->IASetPrimitiveTopology(data.m_PrimitiveTopology);
			//頂点バッファのセット
			pD3D11DeviceContext->IASetVertexBuffers(0, 1, data.m_VertexBuffer.GetAddressOf(), &stride, &offset);
			//インデックスバッファのセット
			pD3D11DeviceContext->IASetIndexBuffer(data.m_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			//各レンダリングステートの設定
			//ブレンドステート
			RenderState->SetBlendState(pD3D11DeviceContext, GetBlendState());
			//デプスステンシルステート
			RenderState->SetDepthStencilState(pD3D11DeviceContext, GetDepthStencilState());
			//テクスチャとサンプラー
			if (shTex) {
				ID3D11ShaderResourceView* srv[2] = { 
					shTex->GetShaderResourceView().Get(),
					m_ShaderResourceView.Get()
					/*m_NormalMap.Get()*/};
				pD3D11DeviceContext->PSSetShaderResources(0, 2, srv);
				//サンプラーを設定
				RenderState->SetSamplerState(pD3D11DeviceContext, GetSamplerState(), 0);
			}
			else {
				//シェーダーリソースもクリア
				pD3D11DeviceContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pNull);
				//サンプラーもクリア
				RenderState->SetSamplerAllClear(pD3D11DeviceContext);
			}
			//影とサンプラー
			if (IsOwnShadowActive()) {
				//シャドウマップのレンダラーターゲット
				auto ShadowmapPtr = Dev->GetShadowMapRenderTarget();
				ID3D11ShaderResourceView* pShadowSRV = ShadowmapPtr->GetShaderResourceView();
				pD3D11DeviceContext->PSSetShaderResources(1, 1, &pShadowSRV);
				//シャドウマップサンプラー
				ID3D11SamplerState* pShadowSampler = RenderState->GetComparisonLinear();
				pD3D11DeviceContext->PSSetSamplers(1, 1, &pShadowSampler);
			}
			//ラスタライザステートと描画
			if (GetRasterizerState() == RasterizerState::DoubleDraw) {
				//透明処理用
				//ラスタライザステート(裏描画)
				pD3D11DeviceContext->RSSetState(RenderState->GetCullFront());
				//描画
				pD3D11DeviceContext->DrawIndexed(data.m_NumIndicis, 0, 0);
				//ラスタライザステート（表描画）
				pD3D11DeviceContext->RSSetState(RenderState->GetCullBack());
				//描画
				pD3D11DeviceContext->DrawIndexed(data.m_NumIndicis, 0, 0);
			}
			else {
				RenderState->SetRasterizerState(pD3D11DeviceContext, GetRasterizerState());
				//描画
				pD3D11DeviceContext->DrawIndexed(data.m_NumIndicis, 0, 0);
			}
		}
	};

	DECLARE_DX11_PIXEL_SHADER(InkDropPixelShaderTest)
	DECLARE_DX11_PIXEL_SHADER(PNTInkPixelShader)
	DECLARE_DX11_VERTEX_SHADER(InkDropVertexShaderTest,VertexPositionTexture)
	DECLARE_DX11_VERTEX_SHADER(PNTInkVertexShader,VertexPositionNormalTexture)
	DECLARE_DX11_CONSTANT_BUFFER(CBBrushData, BrushData)
	DECLARE_DX11_CONSTANT_BUFFER(CBTimeBuffer,TimeBuffer)
}
//end basecross
