/*!
@file UpperDrawComp.cpp
@brief 上部にだけテクスチャを貼る
*/

#include "stdafx.h"
#include "UpperDrawComp.h"
namespace basecross
{
	void UpperDrawComp::OnDraw()
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