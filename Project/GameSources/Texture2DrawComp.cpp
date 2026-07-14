/*!
@file Texture2DrawComp.cpp
@brief テクスチャを二枚貼る
*/

#include "stdafx.h"
#include "Texture2DrawComp.h"
namespace basecross
{
	void Texture2DrawComp::OnDraw()
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
				DrawStatic<VSPNTStaticShadow2, Texture2ShadewPixelSheder>(PtrMeshResource->GetMashData());
			}
			else {
				DrawStatic<VSPNTStatic2, Texture2PixelSheder>(PtrMeshResource->GetMashData());
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
							DrawStatic<VSPNTStaticShadow2, PSPNTStaticShadow2>(vec[i]);
						}
						else {
							DrawStatic<VSPNTStaticShadow2, Texture2ShadewPixelSheder>(vec[i]);
						}
					}
					else {
						DrawStatic<VSPNTStatic2, Texture2PixelSheder>(vec[i]);
					}
				}
			}
		}
		//後始末
		auto Dev = App::GetApp()->GetDeviceResources();
		Dev->InitializeStates();
	}

	IMPLEMENT_DX11_PIXEL_SHADER(Texture2PixelSheder, App::GetApp()->GetShadersPath() + L"PSTexture2.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(Texture2ShadewPixelSheder, App::GetApp()->GetShadersPath() + L"PSTexture2Shadow.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(VSPNTStatic2, App::GetApp()->GetShadersPath() + L"VSPNTStatic2.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(VSPNTStaticShadow2, App::GetApp()->GetShadersPath() + L"VSPNTStaticShadow2.cso")

}
