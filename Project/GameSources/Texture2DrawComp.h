/*!
@file Texture2DrawComp.h
@brief テクスチャを二枚貼る
*/

#pragma once
#include "stdafx.h"
#include "InkDrawComp.h"

namespace basecross
{
	//struct A {
	//	Vec3 k;
	//	int padding;
	//};
	enum class BlendMode {
		Y_Up,	//０：Y軸向き判定
		Z_Axis,	//１：ｚ軸向き判定
	};
	class Texture2DrawComp : public InkDrawComp
	{

		shared_ptr<TextureResource> m_texture;
		BlendMode m_blendMode = BlendMode::Y_Up;

	public:
		Texture2DrawComp(const shared_ptr<GameObject>& draw) :
			InkDrawComp(draw)
		{
		}

		//モード変更の関数
		void SetBlendMode(BlendMode mode) { m_blendMode = mode; }
		virtual void OnDraw() override;
		void SetTexture2(const wstring& TextureKey)
		{
			m_texture = App::GetApp()->GetResource<TextureResource>(TextureKey);
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


			SmCb.ActiveFlg.y = static_cast<float>(m_blendMode);

			//コンスタントバッファの更新
			pD3D11DeviceContext->UpdateSubresource(CBSimple::GetPtr()->GetBuffer(), 0, nullptr, &SmCb, 0, 0);
			//コンスタントバッファの設定
			ID3D11Buffer* pConstantBuffer = CBSimple::GetPtr()->GetBuffer();
			ID3D11Buffer* pNullConstantBuffer = nullptr;

			//インク用コンスタントバッファ
			inkDrawCB InkCb;
			InkCb.Up = GetGameObject()->GetComponent<Transform>()->GetUp();
			
			pD3D11DeviceContext->UpdateSubresource(CBInk::GetPtr()->GetBuffer(), 0, nullptr, &InkCb, 0, 0);
			ID3D11Buffer* InkResourceBuffer = CBInk::GetPtr()->GetBuffer();
			pD3D11DeviceContext->PSSetConstantBuffers(1, 1, &InkResourceBuffer);

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

			auto srv = GetSRV();
			//テクスチャとサンプラー
			if (shTex) {
				pD3D11DeviceContext->PSSetShaderResources(0, 1, shTex->GetShaderResourceView().GetAddressOf());
				pD3D11DeviceContext->PSSetShaderResources(2, 1, m_texture->GetShaderResourceView().GetAddressOf());
				pD3D11DeviceContext->PSSetShaderResources(3, 1, srv.GetAddressOf());

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

	DECLARE_DX11_PIXEL_SHADER(Texture2PixelSheder)
	DECLARE_DX11_PIXEL_SHADER(Texture2ShadewPixelSheder)

}
#pragma once
