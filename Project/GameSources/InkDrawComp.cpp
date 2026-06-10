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
				DrawStatic<InkDrawVertexSheder, InkDrawPixelSheder>(PtrMeshResource->GetMashData());
			}
			else {
				DrawStatic<InkDrawVertexSheder, InkDrawPixelSheder>(PtrMeshResource->GetMashData());
			}
		}
		//後始末
		auto Dev = App::GetApp()->GetDeviceResources();
		Dev->InitializeStates();

	}

	IMPLEMENT_DX11_PIXEL_SHADER(InkDrawPixelSheder, App::GetApp()->GetShadersPath() + L"PSInkDraw.cso")
	IMPLEMENT_DX11_VERTEX_SHADER(InkDrawVertexSheder, App::GetApp()->GetShadersPath() + L"VSInkDraw.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(CBInk)


}