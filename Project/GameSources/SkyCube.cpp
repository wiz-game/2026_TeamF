/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	void SkyCube::OnCreate() {
		auto draw = AddComponent<PTStaticDraw>();
		draw->SetMeshResource(L"DEFAULT_PT_CUBE");
		draw->SetTextureResource(m_TexKey);
		draw->SetRasterizerState(RasterizerState::CullFront);
		draw->SetDepthStencilState(DepthStencilState::None);

		auto transform = GetComponent<Transform>();
		transform->SetScale(Vec3(100));
	}
}
//end basecross
