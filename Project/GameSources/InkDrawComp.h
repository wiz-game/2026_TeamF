#pragma once
#include "stdafx.h"

namespace basecross 
{
	struct inkDrawCB
	{
		Vec4 Up;
	};

	struct cbBrush
	{
		Vec4 centerPositions[4];
		float brushSize = 0;
		int count = 0;
		float textrueWidth = 0;
		float textrueHeight = 0;
	};

	class InkDrawComp : public PNTStaticDraw 
	{
		ComPtr<ID3D11Texture2D> m_texture;//インクのテクスチャ
		ComPtr<ID3D11ShaderResourceView> m_textureSRV;//インクのシェーダーリソースビュー
		ComPtr<ID3D11RenderTargetView> m_textureRTV;//インクのレンダーターゲットビュー
		D3D11_VIEWPORT m_viewport;//インク描画用のビューポート

		cbBrush m_brush;

	public:
		InkDrawComp(const shared_ptr<GameObject>& owner) : 
			PNTStaticDraw(owner) 
		{
		}
		virtual ~InkDrawComp() = default;
		virtual void OnDraw() override;
		virtual void OnCreate() override;
		virtual void OnUpdate() override;
		void CreateTexture(UINT width, UINT height);
		void InkDraw();
		void AddPoint(const Vec3& point);
		void ClearPoint();
		void SetBrushSize(float size)
		{
			m_brush.brushSize = size;
		}


		template<typename T_VShader, typename T_PShader>
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
			pD3D11DeviceContext->PSSetShader(T_PShader::GetPtr()->GetShader(), nullptr, 0);
			//個別処理
			SimpleConstants SmCb;
			//コンスタントバッファの作成
			SetConstants(SmCb, data);
			
			//インク用コンスタントバッファ
			inkDrawCB InkCb;
			InkCb.Up = Vec4(0, 1, 0, 0); // 上方向ベクトルの初期化
			pD3D11DeviceContext->UpdateSubresource(CBInk::GetPtr()->GetBuffer(), 0, nullptr, &InkCb, 0, 0);
			ID3D11Buffer* InkResourceBuffer = CBInk::GetPtr()->GetBuffer();
			pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &InkResourceBuffer);

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
				pD3D11DeviceContext->PSSetShaderResources(0, 1, shTex->GetShaderResourceView().GetAddressOf());
				pD3D11DeviceContext->PSSetShaderResources(2, 1, m_textureSRV.GetAddressOf());

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

	DECLARE_DX11_PIXEL_SHADER(InkDrawPixelSheder)
	DECLARE_DX11_VERTEX_SHADER(InkDrawVertexSheder, VertexPositionNormalTexture)
	DECLARE_DX11_CONSTANT_BUFFER(CBInk, InkDrawComp)
	DECLARE_DX11_PIXEL_SHADER(InkDrawShadowPixelSheder)
	DECLARE_DX11_VERTEX_SHADER(InkDrawShadowVertexSheder, VertexPositionNormalTexture)
	DECLARE_DX11_PIXEL_SHADER(InkDropPixelSheder)
	DECLARE_DX11_VERTEX_SHADER(InkDropVertexSheder, VertexPositionTexture)
	DECLARE_DX11_CONSTANT_BUFFER(CBBrush, cbBrush)

}